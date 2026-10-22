#pragma once
#include "ResourceRegistryBase.h"

namespace cha
{
	/// @brief 音声リソース
	struct FontResource
	{

		HashValue GetHash() const;
	};

	struct FontLoadConfig
	{
		FontLoadConfig(
		    const fs::path& _file,
		    const std::wstring& _familyName,
		    const std::wstring& _localName,
		    const float _fontSize) :
		    file{ _file },
		    familyName{ _familyName },
		    localName{ _localName },
		    fontSize{ _fontSize }
		{
		}

		fs::path file{};                     ///< フォントファイルのパス
		std::wstring familyName{};           ///< フォントファミリー名
		std::wstring localName{ L"ja-jp" };  ///< ja-jp
		float fontSize{};                    ///< フォントサイズ
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
		using LoadConfigT = fs::path;
	};
}  // namespace cha
