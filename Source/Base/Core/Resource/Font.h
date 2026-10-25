#pragma once
#include "ResourceRegistryBase.h"

namespace cha
{
	/// @brief 音声リソース
	struct FontResource
	{
		ComPtr<IDWriteTextFormat> pTextFormat{};
	};

	/// @brief フォントの配置 8方位
	using FontAlignment = int32_t;
	/// @brief フォントの配置 8方位
	enum FontAlignment_
	{
		FontAlignment_TopLeft,      ///< 左上
		FontAlignment_Top,          ///< 上
		FontAlignment_TopRight,     ///< 右上
		FontAlignment_Left,         ///< 左
		FontAlignment_Center,       ///< 中央
		FontAlignment_Right,        ///< 右
		FontAlignment_BottomLeft,   ///< 左下
		FontAlignment_Bottom,       ///< 下
		FontAlignment_BottomRight,  ///< 右下
	};

	struct FontLoadConfig
	{
		FontLoadConfig(
		    const fs::path& _file           = "Font/YuseiMagic/YuseiMagic-Regular.ttf",
		    const std::wstring& _familyName = L"Yusei Magic",
		    const float _fontSize           = 32.0f,
		    const std::wstring& _localName  = L"ja-jp",
		    const bool _isUniqueLoad        = false) :
		    fontSize{ _fontSize },
		    file{ _file },
		    familyName{ _familyName },
		    localName{ _localName },
		    isUniqueLoad{ _isUniqueLoad }
		{
		}

		HashValue GetHash() const
		{
			if (isUniqueLoad)
			{
				return 0;  // ユニークロードが指定されているなら必ず読み込むように0を返しちゃう
			}
			HashValue hashValue{};
			HashCombine(hashValue, fontSize);
			HashCombine(hashValue, file.string());
			HashCombine(hashValue, familyName);
			HashCombine(hashValue, localName);
			HashCombine(hashValue, weight);
			HashCombine(hashValue, style);
			HashCombine(hashValue, stretch);
			HashCombine(hashValue, textAlignment);
			HashCombine(hashValue, paragraphAlignment);
			return hashValue;
		}

		FontLoadConfig& WithFontSize(
		    const float _fontSize)
		{
			fontSize = _fontSize;
			return *this;
		}
		FontLoadConfig& WithWeight(
		    const DWRITE_FONT_WEIGHT _weight)
		{
			weight = _weight;
			return *this;
		}
		FontLoadConfig& WithStyle(
		    const DWRITE_FONT_STYLE _style)
		{
			style = _style;
			return *this;
		}
		FontLoadConfig& WithStretch(
		    const DWRITE_FONT_STRETCH _stretch)
		{
			stretch = _stretch;
			return *this;
		}
		FontLoadConfig& WithTextAlignment(
		    const DWRITE_TEXT_ALIGNMENT _textAlignment)
		{
			textAlignment = _textAlignment;
			return *this;
		}
		FontLoadConfig& WithParagraphAlignment(
		    const DWRITE_PARAGRAPH_ALIGNMENT _paragraphAlignment)
		{
			paragraphAlignment = _paragraphAlignment;
			return *this;
		}
		FontLoadConfig& WithIsUniqueLoad(
		    const bool _isUniqueLoad)
		{
			isUniqueLoad = _isUniqueLoad;
			return *this;
		}
		FontLoadConfig& WithFile(
		    const fs::path& _file)
		{
			file = _file;
			return *this;
		}
		FontLoadConfig& WithFamilyName(
		    const std::wstring& _familyName)
		{
			familyName = _familyName;
			return *this;
		}
		FontLoadConfig& WithLocalName(
		    const std::wstring& _localName)
		{
			localName = _localName;
			return *this;
		}
		FontLoadConfig& WithFontAlignment(
		    const FontAlignment _fontAlignment)
		{
			textAlignment = (_fontAlignment == FontAlignment_TopLeft || _fontAlignment == FontAlignment_Left ||
			                 _fontAlignment == FontAlignment_BottomLeft)
			                    ? DWRITE_TEXT_ALIGNMENT_LEADING
			                : (_fontAlignment == FontAlignment_Top || _fontAlignment == FontAlignment_Center ||
			                   _fontAlignment == FontAlignment_Bottom)
			                    ? DWRITE_TEXT_ALIGNMENT_CENTER
			                : (_fontAlignment == FontAlignment_TopRight || _fontAlignment == FontAlignment_Right ||
			                   _fontAlignment == FontAlignment_BottomRight)
			                    ? DWRITE_TEXT_ALIGNMENT_TRAILING
			                    : DWRITE_TEXT_ALIGNMENT_JUSTIFIED;

			paragraphAlignment =
			    (_fontAlignment == FontAlignment_TopLeft || _fontAlignment == FontAlignment_Top ||
			     _fontAlignment == FontAlignment_TopRight)
			        ? DWRITE_PARAGRAPH_ALIGNMENT_NEAR
			    : (_fontAlignment == FontAlignment_Left || _fontAlignment == FontAlignment_Center ||
			       _fontAlignment == FontAlignment_Right)
			        ? DWRITE_PARAGRAPH_ALIGNMENT_CENTER
			    : (_fontAlignment == FontAlignment_BottomLeft || _fontAlignment == FontAlignment_Bottom ||
			       _fontAlignment == FontAlignment_BottomRight)
			        ? DWRITE_PARAGRAPH_ALIGNMENT_FAR
			        : DWRITE_PARAGRAPH_ALIGNMENT_CENTER;

			return *this;
		}

		fs::path file{};                                                                   ///< フォントファイルのパス
		std::wstring familyName{};                                                         ///< フォントファミリー名
		std::wstring localName{ L"ja-jp" };                                                ///< ja-jp
		float fontSize{};                                                                  ///< フォントサイズ
		DWRITE_FONT_WEIGHT weight{ DWRITE_FONT_WEIGHT_NORMAL };                            ///< 幅
		DWRITE_FONT_STYLE style{ DWRITE_FONT_STYLE_NORMAL };                               ///<
		DWRITE_FONT_STRETCH stretch{ DWRITE_FONT_STRETCH_NORMAL };                         ///<
		DWRITE_TEXT_ALIGNMENT textAlignment{ DWRITE_TEXT_ALIGNMENT_LEADING };              ///< 横方向の配置
		DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment{ DWRITE_PARAGRAPH_ALIGNMENT_NEAR };  ///< 縦方向の配置
		bool isUniqueLoad{ false };                                                        ///< リソース
	};

	/// @brief 音声リソースを格納
	class FontRegistry : public ResourceRegistryBase<FontResource, FontLoadConfig>
	{
	  public:
		FontRegistry()  = default;
		~FontRegistry() = default;

		/// @brief 試しにフォントファイルをロードする
		/// @param _config 読み込む設定
		/// @param _out ロード出力先
		/// @return ロードに成功 true / false
		bool TryLoad(FontLoadConfig&& _config, FontResource& _out) override;
	};

	template <> struct ResourceTraits<ResourceType::Font>
	{
		using DataT       = FontResource;
		using RegistryT   = FontRegistry;
		using LoadConfigT = FontLoadConfig;
	};
}  // namespace cha
