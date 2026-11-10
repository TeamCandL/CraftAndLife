#include "Camera.hlsli"
#include "Transform.hlsli"
#include "ModelTransform.hlsli"
#include "Vertex/ModelVertex.hlsli"
#include "Material/ModelMaterial.hlsli"

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

#define MAX_BONES 256

cbuffer BoneBuffer : register(b4)
{
	matrix BoneMatrices[MAX_BONES];
};

// 輪郭の太さ
static const float EdgeWidth = 0.07f;

struct VSOutput
{
	float4 pos : SV_Position;
	float2 uv : TEXCOORD0; // UV座標を追加
};

VSOutput VS(ModelVertex input)
{
	VSOutput output;

	float4 localPos = input.pos;
	float3 localNormal = input.normal.xyz;

    // スキニング対応
	if (ModelMaterial_HasTexture_HasSkin_HasAnim.y)
	{
		matrix skinMatrix =
			BoneMatrices[input.boneIndices.x] * input.boneWeights.x +
			BoneMatrices[input.boneIndices.y] * input.boneWeights.y +
			BoneMatrices[input.boneIndices.z] * input.boneWeights.z +
			BoneMatrices[input.boneIndices.w] * input.boneWeights.w;

		localPos = mul(localPos, skinMatrix);

		localNormal = mul(
			float4(localNormal, 0.0f),
			skinMatrix
		).xyz;
	}

    // 頂点を法線方向に膨らませる
	localPos.xyz += normalize(localNormal) * EdgeWidth;

    // ワールド変換
	float4 worldPos = mul(localPos, ModelTransform_ToWorldMatrix);
	worldPos = mul(worldPos, Transform_World);

	output.pos = mul(worldPos, Camera_ViewProjection);
	output.uv = input.uv; // UVをピクセルシェーダーに渡す

	return output;
}

float4 PS(VSOutput input) : SV_Target0
{
	// テクスチャがある場合はアルファ（透明度）をチェックして抜き取る
	[branch]
	if (ModelMaterial_HasTexture_HasSkin_HasAnim.x > 0.5f)
	{
		float4 texColor = g_texture.Sample(g_sampler, input.uv);
		// 透明な部分（アルファが低い部分）はエッジを描画しないように削る
		if (texColor.a < 0.1f)
		{
			clip(-1.0f);
		}
	}

	// 綺麗な黄色を返す
	return float4(1.0f, 1.0f, 0.0f, 1.0f);
}
