#pragma once
#include <math.h>
#include <cmath>

struct Math
{
	/// @brief 三平方の定理を使って、2点間の距離を求める。
	/// @param num1 辺の長さ1
	/// @param num2 辺の長さ2
	/// @return 斜辺の長さ
	static float GetThreeSquares(float num1, float num2)
	{
		return sqrt(num1 * num1 + num2 * num2);
	}

	/// @brief 辺の変化の割合を求める。
	/// @param num1 辺１、変化の割合を求めたい辺の長さ
	/// @param num2 辺２、もう１つの辺の長さ
	/// @return 辺１の変化の割合
	static float GetSquareVec(float num1, float num2)
	{
		float length = GetThreeSquares(num1, num2);

		// 長さが0なら、方向ベクトルを0にする
		if (length <= 0.00001f)
		{
			return 0.0f;
		}

		return num1 / length;
	}

	/// @brief 角度を求める。
	/// @param num1 目的地までのx軸の距離
	/// @param num2	目的地までのz軸の距離
	/// @return 角度
	static float GetAngle(float x, float z)
	{
		return atan2f(x, z);
	}

	static float GetDeg(float angle_)
	{
		return angle_ * 180 / DirectX::XM_PI;
	}
};