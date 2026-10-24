#pragma once
#include "ResourceRegistryBase.h"

namespace cha
{
	/// @brief 音声リソース
	struct FontResource
	{
		ComPtr<IDWriteTextFormat> pTextFormat{};
		HashValue GetHash() const;
	};

	struct FontLoadConfig
	{
		/*FontLoadConfig() :
		    FontLoadConfig
		    {
		        "Font/YuseiMagic/YuseiMagic-Regular.ttf",
		        L"Yusei Magic",
		        32.0f,
		        L"ja-jp"
		    }
		{

		}*/

		FontLoadConfig(
		    const fs::path& _file           = "Font/YuseiMagic/YuseiMagic-Regular.ttf",
		    const std::wstring& _familyName = L"Yusei Magic",
		    const float _fontSize           = 32.0f,
		    const std::wstring& _localName  = L"ja-jp") :
		    fontSize{ _fontSize },
		    file{ _file },
		    familyName{ _familyName },
		    localName{ _localName }
		{
			VCU_LOG("フォント作成size:{}, file:{}", fontSize, file.string());
		}

		float fontSize{};                                           ///< フォントサイズ
		fs::path file{};                                            ///< フォントファイルのパス
		std::wstring familyName{};                                  ///< フォントファミリー名
		std::wstring localName{ L"ja-jp" };                         ///< ja-jp
		DWRITE_FONT_WEIGHT weight{ DWRITE_FONT_WEIGHT_NORMAL };     ///< 幅
		DWRITE_FONT_STYLE style{ DWRITE_FONT_STYLE_NORMAL };        ///<
		DWRITE_FONT_STRETCH stretch{ DWRITE_FONT_STRETCH_NORMAL };  ///<
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
