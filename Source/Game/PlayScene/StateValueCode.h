#pragma once
#include "StateRegistry.h"
#include <vector>
#include <cstdint>
#include <cstring>

/// @brief ステート値のエンコード・デコード
struct StateValueCode
{
	/// @brief 1ステートあたりのデータサイズ
	static constexpr size_t ValueSize = 5;

	/// @brief 型情報を取得
	static uint8_t GetType(
	    const bool&)
	{
		return 0;
	}
	static uint8_t GetType(
	    const uint8_t&)
	{
		return 1;
	}
	static uint8_t GetType(
	    const int32_t&)
	{
		return 2;
	}
	static uint8_t GetType(
	    const float&)
	{
		return 3;
	}

	/// @brief 値をバイト配列に追加
	static void EncodeValue(
	    const bool& value,
	    std::vector<uint8_t>& output)
	{
		output.push_back(value ? 1 : 0);
		output.insert(output.end(), 3, 0);
	}

	static void EncodeValue(
	    const uint8_t& value,
	    std::vector<uint8_t>& output)
	{
		output.push_back(value);
		output.insert(output.end(), 3, 0);
	}

	static void EncodeValue(
	    const int32_t& value,
	    std::vector<uint8_t>& output)
	{
		uint8_t bytes[sizeof(int32_t)]{};
		std::memcpy(bytes, &value, sizeof(int32_t));

		output.insert(output.end(), bytes, bytes + sizeof(int32_t));
	}

	static void EncodeValue(
	    const float& value,
	    std::vector<uint8_t>& output)
	{
		uint8_t bytes[sizeof(float)]{};
		std::memcpy(bytes, &value, sizeof(float));

		output.insert(output.end(), bytes, bytes + sizeof(float));
	}

	/// @brief ステート値をバイト配列に変換
	static bool Encode(
	    const StateValue& value,
	    std::vector<uint8_t>& output)
	{
		std::visit(
		    [&](const auto& v)
		    {
			    output.push_back(GetType(v));
			    EncodeValue(v, output);
		    },
		    value);

		return true;
	}

	/// @brief バイト配列をステート値に変換
	static bool Decode(
	    const uint8_t* data,
	    size_t size,
	    StateValue& output)
	{
		if (data == nullptr || size < ValueSize)
		{
			return false;
		}

		switch (data[0])
		{
		case 0:
			output = (data[1] != 0);
			return true;

		case 1:
			output = data[1];
			return true;

		case 2:
		{
			int32_t value{};
			std::memcpy(&value, data + 1, sizeof(int32_t));
			output = value;
			return true;
		}

		case 3:
		{
			float value{};
			std::memcpy(&value, data + 1, sizeof(float));
			output = value;
			return true;
		}

		default:
			return false;
		}
	}
};