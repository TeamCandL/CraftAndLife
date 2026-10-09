#pragma once
#include <Game/Game.h>

/// @brief メンバー要素のデータ
struct MemberElementData
{
	MemberElementData(const Vec2 _position, const Vec2 _scale) :
		transform{ _position, _scale }
	{
	}

	Component<UITransform> transform{};  ///< UI座標系
	Component<UIText> text{};            ///< UIテキスト
};

/// @brief メンバー要素
struct MemberElement : ObjectBase<MemberElementData>
{
	using ObjectBase<MemberElementData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;

	/// @brief 名前をセットする
	/// @param _name 名前の文字列
	void SetName(const std::string_view _name);

	/// @brief 名前を取得する
	/// @return 名前の文字列
	std::string_view GetName() const { return text.content; }
};
