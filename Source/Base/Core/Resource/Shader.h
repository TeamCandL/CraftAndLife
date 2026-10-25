#pragma once
#include "ResourceRegistryBase.h"
#include "Shader/ShaderCompileConfig.h"
#include "Shader/RasterizerConfig.h"

namespace cha
{
	/// @brief シェーダリソース
	struct ShaderResource
	{
		ComPtr<ID3D11VertexShader> pVertexShader{};      // 頂点シェーダ
		ComPtr<ID3D11PixelShader> pPixelShader{};        // ピクセルシェーダ
		ComPtr<ID3D11InputLayout> pInputLayout{};        // インプットレイアウト
		ComPtr<ID3D11RasterizerState> pRasterizerState;  // ラスタライザ
		RasterizerConfig rasterizerConfig{};             // ラスタライザの設定
	};

	/// @brief シェーダ読み込み設定
	struct ShaderLoadConfig
	{
		inline ShaderLoadConfig(
		    const fs::path& _file,
		    const RasterizerConfig _rasterizerConfig = {}) :
		    file{ _file },
		    rasterizerConfig{ _rasterizerConfig }
		{
		}

		fs::path file{};
		RasterizerConfig rasterizerConfig{};

		HashValue GetHash() const
		{
			HashValue hashValue{};
			HashCombine(hashValue, file.string());
			HashCombine(hashValue, rasterizerConfig.cullMode);
			HashCombine(hashValue, rasterizerConfig.fillMode);
			HashCombine(hashValue, rasterizerConfig.frontCounterClockwise);
			return hashValue;
		}
	};

	/// @brief シェーダリソースを格納
	class ShaderRegistry : public ResourceRegistryBase<ShaderResource, ShaderLoadConfig>
	{
	  public:
		ShaderRegistry()  = default;
		~ShaderRegistry() = default;

		/// @brief 試しにシェーダをロードする
		/// @param _config 読み込み設定
		/// @param _out ロード出力先
		/// @return ロードに成功 true / false
		bool TryLoad(ShaderLoadConfig&& _config, ShaderResource& _out) override;

	  private:
		ShaderCompileConfig compileConfig_{};  // コンパイル時の設定
	};

	template <> struct ResourceTraits<ResourceType::Shader>
	{
		using DataT       = ShaderResource;
		using RegistryT   = ShaderRegistry;
		using LoadConfigT = fs::path;
	};
}  // namespace cha
