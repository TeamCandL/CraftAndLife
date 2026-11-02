#pragma once

#include <Game/Game.h>
namespace UI
{
	struct ListElementData
	{
		ListElementData(
		    const std::string_view _normal,
		    const std::string_view _hover,
		    const std::string_view _label,
		    const std::string_view _value,
		    const Vec2 _begin,
		    const Vec2 _scale,
		    Resource<ResourceType::Font> _font = {},
		    const Color _color                 = Color::BLACK) :
		    textureNormal{ _normal },
		    textureHover{ _hover },
		    transform{ _begin, _scale },
		    sprite{ _normal },
		    text{ _font, _color, _label },
		    value{ _value }
		{
			transform.layerOrder = 1;
		}

		Resource<ResourceType::Font> font{};              // 文字フォント
		Resource<ResourceType::Texture> textureNormal{};  // 普通
		Resource<ResourceType::Texture> textureHover{};   // カーソルが当たってる

		Component<UITransform> transform{};   // UI座標系
		Component<UIImage> sprite{};          // UI画像
		Component<UIRaycastTarget> target{};  // 当たり判定用
		Component<UIText> text{};             // テキスト

		std::string content{};  // 表示内容
		std::string value{};    // 内部に持たせる文字列
	};
	struct ListElement : ObjectBase<ListElementData>
	{
		using ObjectBase<ListElementData>::ObjectBase;
		void Update(bool _selected);
	};

	struct ListBoxData
	{
		// リストボックスの背景画像用のコンポーネント
		Component<UITransform> transform{};  // UI座標系
		Component<UIImage> sprite{};         // UI画像
	  protected:
		std::vector<GameObject> elements;
		int currentIndex;
	};
	struct ListBox : ObjectBase<ListBoxData>
	{
		void Start() override;
		void Update() override;
		std::string GetSelectionData();
		bool IsPushed();
		void PushBackElement(const ListElement& _element);
		void SetActiveAll(const bool _isActive);

	  private:
		void MoveFocus(int _dir);
	};
}  // namespace UI