
// ToonEdge.hlsl
// トゥーンシェーダー用の輪郭描画シェーダー
// 最初に輪郭を描画し、その後 Toon.hlsl で本体を描画する。

#include "Camera.hlsli"
#include "Transform.hlsli"
#include "Material/ModelMaterial.hlsli"
#include "ModelTransform.hlsli"
#include "Vertex/ModelVertex.hlsli"

// ============================================================
// ボーン
// Toon.hlsl と同じ定数バッファ構成
// ============================================================

#define MAX_BONES 256

cbuffer BoneBuffer : register(b4)
{
	matrix BoneMatrices[MAX_BONES];
};

// ============================================================
// 輪郭設定
// ============================================================

// モデルのワールド空間での輪郭の太さ。
// モデルのサイズに合わせて調整する。
static const float OUTLINE_WIDTH = 0.02f;

// ============================================================
// 頂点シェーダー
// ============================================================

float4 VS(ModelVertex input) : SV_Position
{
	float4 localPos;
	float3 localNormal;

    // --------------------------------------------------------
    // スキニング・アニメーション
    // --------------------------------------------------------

	if (ModelMaterial_HasTexture_HasSkin_HasAnim.y > 0.5f)
	{
		matrix skinMatrix =
            BoneMatrices[input.boneIndices.x] * input.boneWeights.x +
            BoneMatrices[input.boneIndices.y] * input.boneWeights.y +
            BoneMatrices[input.boneIndices.z] * input.boneWeights.z +
            BoneMatrices[input.boneIndices.w] * input.boneWeights.w;

        // アニメーション後の頂点座標
		localPos = mul(input.pos, skinMatrix);

        // アニメーション後の法線
		localNormal = mul(
            float4(input.normal.xyz, 0.0f),
            skinMatrix
        ).xyz;
	}
	else
	{
		localPos = input.pos;
		localNormal = input.normal.xyz;
	}

    // --------------------------------------------------------
    // ワールド座標への変換
    // Toon.hlsl と同じ変換順序
    // --------------------------------------------------------

	float4 worldPos = mul(
        localPos,
        ModelTransform_ToWorldMatrix
    );

	worldPos = mul(worldPos, Transform_World);

    // 法線は方向ベクトルとして変換するため w = 0
	float3 worldNormal = mul(
        float4(localNormal, 0.0f),
        ModelTransform_ToWorldMatrix
    ).xyz;

	worldNormal = mul(
        float4(worldNormal, 0.0f),
        Transform_World
    ).xyz;

	worldNormal = normalize(worldNormal);

    // --------------------------------------------------------
    // 輪郭を作るためにモデルを膨らませる
    // --------------------------------------------------------

	worldPos.xyz += worldNormal * OUTLINE_WIDTH;

    // --------------------------------------------------------
    // カメラ変換
    // --------------------------------------------------------

	float4 clipPos = mul(
        worldPos,
        Camera_ViewProjection
    );

	return clipPos;
}

// ============================================================
// ピクセルシェーダー
// ============================================================

float4 PS() : SV_Target0
{
    // 輪郭色
	return float4(0.0f, 0.0f, 0.0f, 1.0f);
}
