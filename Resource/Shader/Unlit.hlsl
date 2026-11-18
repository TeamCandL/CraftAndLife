#include "Camera.hlsli"
#include "Transform.hlsli"
#include "Material/ModelMaterial.hlsli"
#include "ModelTransform.hlsli"
#include "Vertex/ModelVertex.hlsli"

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

#define MAX_BONES 256
cbuffer BoneBuffer : register(b4)
{
	matrix BoneMatrices[MAX_BONES];
};

struct VSOutput
{
	float4 pos : SV_Position; // 頂点座標
	float3 worldPos : TEXCOORD1;  // ワールド座標
	float4 normal : NORMAL0;  // 法線
	float2 uv : TEXCOORD0; // UV座標
};

VSOutput VS(ModelVertex input)
{
	VSOutput output;

	float4 worldPos;
	float4 worldNormal;
	if (ModelMaterial_HasTexture_HasSkin_HasAnim.y)
	{
		matrix skinMatrix =
        BoneMatrices[input.boneIndices.x] * input.boneWeights.x +
        BoneMatrices[input.boneIndices.y] * input.boneWeights.y +
        BoneMatrices[input.boneIndices.z] * input.boneWeights.z +
        BoneMatrices[input.boneIndices.w] * input.boneWeights.w;

		worldPos = mul(input.pos, skinMatrix);
		worldNormal = mul(float4(input.normal.xyz, 0.0f), skinMatrix);
	}
	else
	{
		worldPos = input.pos;
		worldNormal = input.normal;
	}

// モデル変換
	worldPos = mul(worldPos, ModelTransform_ToWorldMatrix);
	worldNormal = mul(worldNormal, ModelTransform_ToWorldMatrix);

// ワールド変換
	worldPos = mul(worldPos, Transform_World);
	worldNormal = mul(worldNormal, Transform_World);

	output.normal = normalize(worldNormal);

	output.worldPos = worldPos.xyz;
	
	output.pos = mul(worldPos, Camera_ViewProjection);
	
	output.uv = input.uv;

	
	
	return output;
}

// 4x4ベイヤーディザ行列
float GetBayerDitherValue(uint2 screenPos)
{
	const float bayerMatrix[16] =  // TODO: 変換内容はわかりまへん
	{
		0.0f / 16.0f, 8.0f / 16.0f, 2.0f / 16.0f, 10.0f / 16.0f,
		12.0f / 16.0f, 4.0f / 16.0f, 14.0f / 16.0f, 6.0f / 16.0f,
		3.0f / 16.0f, 11.0f / 16.0f, 1.0f / 16.0f, 9.0f / 16.0f,
		15.0f / 16.0f, 7.0f / 16.0f, 13.0f / 16.0f, 5.0f / 16.0f
	};

	uint x = screenPos.x % 4;
	uint y = screenPos.y % 4;
	return bayerMatrix[y * 4 + x];
}

float4 PS(VSOutput input) : SV_Target0
{
	{  // ディザ透過処理
		// カメラの位置を取得
		float3 cameraPos = Camera_Position.xyz; // もしくはカメラインクルード内の変数

		// カメラとピクセルの距離を計算
		float distToCamera = distance(cameraPos, input.worldPos.xyz);

		// 距離に応じたフェード係数 0.0f..1.0f を計算
		float minDist = 0.5f;
		float maxDist = 1.5f;
		float ditherAlpha = saturate((distToCamera - minDist) / (maxDist - minDist));

		// ベイヤーディザによるクリッピング判定
		uint2 screenPos = (uint2)input.pos.xy;
		float ditherThreshold = GetBayerDitherValue(screenPos);

		if (ditherAlpha < ditherThreshold)
		{
			clip(-1.0f); // カメラに近づいたピクセルをドット状に削る
		}
	}
	
	float4 diffuseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	// 偽の時絶対にテクスチャをサンプルしなくなる！らしい！←結果ダメだった
	[branch]
	if (ModelMaterial_HasTexture_HasSkin_HasAnim.x > 0.5f)
	{
		diffuseColor = g_texture.Sample(g_sampler, input.uv);
	}
	else
	{
		diffuseColor = ModelMaterial_BaseColor;
	}
	return diffuseColor;
}
