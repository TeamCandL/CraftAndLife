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

float4 PS(VSOutput input) : SV_Target0
{
	// 中心に移動
	float2 st = input.uv.xy - float2(0.5f, 0.5f);

	// y軸反転
	st.y = -st.y;
	
	float angleRad = atan2(st.x, st.y);

	// 角度が0未満なら
	if (angleRad < 0.0f)
	{
		angleRad += TAU;
	}

	static const float MIN_RADIUS = UIImageCircleMaterial_MinRadius_MaxRadius_BeginAngleRad_EndAngleRad.x / SpriteRendererMaterial_ImageSizeX_ImageSizeY.x;
	static const float MAX_RADIUS = UIImageCircleMaterial_MinRadius_MaxRadius_BeginAngleRad_EndAngleRad.y / SpriteRendererMaterial_ImageSizeX_ImageSizeY.y;

	// ピクセルの中心からの距離が指定角度範囲外なら
	if (length(st) < MIN_RADIUS || MAX_RADIUS < length(st))
	{
		// 無視！
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	
	// ピクセルの角度が指定角度範囲外なら
	if (angleRad < UIImageCircleMaterial_MinRadius_MaxRadius_BeginAngleRad_EndAngleRad.z ||
		UIImageCircleMaterial_MinRadius_MaxRadius_BeginAngleRad_EndAngleRad.w < angleRad)
	{
		// 無視！
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	
	float4 diffuse;
	if (SpriteRendererMaterial_HasTexture.x)
	{
		diffuse = g_texture.Sample(g_sampler, input.uv);
	}
	else
	{
		diffuse = SpriteRendererMaterial_BaseColor;
	}
	return diffuse;
}
