#pragma once
#include <DirectXMath.h>
#include <cmath>

/// @brief Geminiライブラリ-数学部門
namespace GemiMath
{
	/// @brief 2つの3Dベクトルのなす角が指定角度（ラジアン）以内かを外積で判定する
	/// @param vecA 判定対象ベクトルA
	/// @param vecB 判定対象ベクトルB
	/// @param maxAngleRad 許容する最大角度（ラジアン、0 以上 PI/2 未満を想定）
	/// @return なす角が maxAngleRad 以内なら true、それ以外は false
	bool IsAngleWithinRadiansByCross3D(
	    DirectX::FXMVECTOR vecA,
	    DirectX::FXMVECTOR vecB,
	    float maxAngleRad)
	{
		const float lenSqA{ DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(vecA)) };
		const float lenSqB{ DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(vecB)) };

		if (lenSqA <= FLT_EPSILON || lenSqB <= FLT_EPSILON)
		{
			return false;
		}

		const DirectX::XMVECTOR normA{ DirectX::XMVector3Normalize(vecA) };
		const DirectX::XMVECTOR normB{ DirectX::XMVector3Normalize(vecB) };

		// 180度付近の鈍角を除外するため、内積が正（90度未満）であることを確認
		const float dot{ DirectX::XMVectorGetX(DirectX::XMVector3Dot(normA, normB)) };
		if (dot <= 0.0f)
		{
			return false;
		}

		// 正規化ベクトル同士の外積の長さ = sin(theta)
		const DirectX::XMVECTOR cross{ DirectX::XMVector3Cross(normA, normB) };
		const float sinTheta{ DirectX::XMVectorGetX(DirectX::XMVector3Length(cross)) };
		const float maxSin{ std::sin(maxAngleRad) };

		constexpr float kMargin{ 1e-5f };
		return sinTheta <= (maxSin + kMargin);
	}
}

