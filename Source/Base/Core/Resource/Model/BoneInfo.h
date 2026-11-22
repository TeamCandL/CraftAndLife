#pragma once
#include <string>

namespace cha
{
	/// @brief ボーンの情報
	struct BoneInfo
	{
		std::string name{};
		int parentIndex{};        // 親ボーンの配列インデックス
		Mat4x4 invBindMatrix{};   // 逆バインドポーズ行列
		Mat4x4 localTransform{};  // 初期ポーズのTRS
	};
}  // namespace cha
