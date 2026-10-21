#pragma once
#include <math.h>

struct Math
{
	// 三平方の定理。斜辺の値を返します
	static float GetThreeSquares(float num1, float num2)
	{
		return sqrt(num1 * num1 + num2 * num2);
	}

	/*
	num1には、軸に対する変化の割合を求めたい値を入れる。
	x軸に対する変化の割合を求めたい場合は、num1にx軸の変化量を入れ、num2にy軸の変化量を入れる。
	y軸に対する変化の割合を求めたい場合は、num1にy軸の変化量を入れ、num2にx軸の変化量を入れる。
	*/
	static float GetSquareVec(float num1, float num2)
	{
		return 0.0f;
		//return num1 / GetThreeSquares(num1 * num1 + num2 * num2);
	}
	/// キャラの向いている方向を求める。
	static float GetAngle(float num1, float num2)
	{
		return atan2f(num1, num2);
	}
};