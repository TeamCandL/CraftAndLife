#include "Camera.hlsli"
#include "Transform.hlsli"
#include "ModelTransform.hlsli"
#include "Vertex/ModelVertex.hlsli"
#include "Material/ModelMaterial.hlsli"

#define MAX_BONES 256

cbuffer BoneBuffer : register(b4)
{
	matrix BoneMatrices[MAX_BONES];
};

// ★ C++側からのバインド忘れを防ぐため、太さをここで直接定義します（必要に応じて数値を調整してください）
static const float EdgeWidth = 0.02f;

struct VSOutput
{
	float4 pos : SV_Position;
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

	return output;
}

float4 PS(VSOutput input) : SV_Target0
{
	// 輪郭の色（黒。お好みで変更してください）
	return float4(1.0f, 1.0f, 0.0f, 1.0f);
}
