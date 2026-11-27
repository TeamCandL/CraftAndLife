#include "UITransform.hlsli"
#include "Material/SpriteRendererMaterial.hlsli"
#include "Material/UIImageCircleMaterial.hlsli"

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

struct VSInput
{
	float4 pos : POSITION;
	float2 uv : TEXCOORD0;
};

struct VSOutput
{
	float4 pos : SV_Position; // 頂点座標
	float2 uv : TEXCOORD0; // UV座標
};

// 2 pi
static const float TAU = acos(-1.0f) * 2.0f;

VSOutput VS(VSInput input)
{
	VSOutput output;
	float4 pos = float4(input.pos.xyz, 1.0f);

	pos = mul(pos, UITransform_NDC);
	
	output.pos = pos;
	output.uv = input.uv;
	return output;
}

// 角度を [0, TAU) に正規化するヘルパー
float WrapAngle0ToTau(float angle)
{
	angle = fmod(angle, TAU);
	if (angle < 0.0f)
	{
		angle += TAU;
	}
	return angle;
}

float4 PS(VSOutput input) : SV_Target0
{
	// 中心に移動（UV: 左上(0,0) -> 右下(1,1)）
	float2 st = input.uv.xy - float2(0.5f, 0.5f);

	// y軸反転（上方向を +y とする）
	st.y = -st.y;

	// 上方向(0, +1)を 0 rad とし、時計回りに [0, TAU) へ正規化
	float pixelAngle = atan2(st.x, st.y);
	pixelAngle = WrapAngle0ToTau(pixelAngle);

	// 半径の正規化
	const float minRadius =
		UIImageCircleMaterial_MinRadius_MaxRadius_BeginAngleRad_SizeAngleRad.x /
		SpriteRendererMaterial_ImageSizeX_ImageSizeY.x;
	const float maxRadius =
		UIImageCircleMaterial_MinRadius_MaxRadius_BeginAngleRad_SizeAngleRad.y /
		SpriteRendererMaterial_ImageSizeX_ImageSizeY.y;

	const float dist = length(st);
	if (dist < minRadius || maxRadius < dist)
	{
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	const float beginAngle = WrapAngle0ToTau(
		UIImageCircleMaterial_MinRadius_MaxRadius_BeginAngleRad_SizeAngleRad.z);
	const float sizeAngle =
		UIImageCircleMaterial_MinRadius_MaxRadius_BeginAngleRad_SizeAngleRad.w;

	// 1周（またはそれ以上）指定されている場合は角度範囲をすべて許可
	if (abs(sizeAngle) < TAU)
	{
		// beginAngle を 0 としたときのピクセルの相対角度 [0, TAU)
		const float relAngle = WrapAngle0ToTau(pixelAngle - beginAngle);

		if (sizeAngle >= 0.0f)
		{
			// 時計回りに sizeAngle 分だけ描画
			if (relAngle > sizeAngle)
			{
				return float4(0.0f, 0.0f, 0.0f, 0.0f);
			}
		}
		else
		{
			// 反時計回りに |sizeAngle| 分だけ描画（relAngle を [-TAU, 0] に変換）
			const float signedRelAngle = relAngle - TAU;
			if (signedRelAngle < sizeAngle)
			{
				return float4(0.0f, 0.0f, 0.0f, 0.0f);
			}
		}
	}

	float4 diffuse;
	if (SpriteRendererMaterial_HasTexture.x)
	{
		diffuse = g_texture.Sample(g_sampler, input.uv);
		diffuse.rgb *= SpriteRendererMaterial_BaseColor.rgb;
		diffuse.a *= SpriteRendererMaterial_BaseColor.a;
	}
	else
	{
		diffuse = SpriteRendererMaterial_BaseColor;
	}
	diffuse += SpriteRendererMaterial_AddColor;

	return diffuse;
}
