#pragma once
#include "ResourceRegistryBase.h"
#include "Model/ModelParts.h"
#include "Model/ModelVertex.h"
#include "../Resource.h"
#include "Material.h"
#include <ufbx/ufbx.h>
#include "Model/AnimationClip.h"
#include "Model/BoneInfo.h"

namespace cha
{
	/// @brief モデルリソース
	struct ModelResource
	{
		ModelResource() = default;

		// コピーは禁止
		ModelResource(const ModelResource&)            = delete;
		ModelResource& operator=(const ModelResource&) = delete;

		// ムーブを許可
		ModelResource(
		    ModelResource&& _other) noexcept :
		    file{ std::move(_other.file) },
		    subParts{ std::move(_other.subParts) },
		    isSkinnedModel{ _other.isSkinnedModel },
		    bones{ std::move(_other.bones) },
		    animations{ std::move(_other.animations) },
		    playingAnimIndex{ _other.playingAnimIndex },
		    currentAnimTime{ _other.currentAnimTime },
		    bakedAnimations{ std::move(_other.bakedAnimations) },
		    pRawScene{ _other.pRawScene },  // ufbxのシーンポインタは引き継ぐ
		    pBoneConstantBuffer{ std::move(_other.pBoneConstantBuffer) }
		{
			_other.bakedAnimations.clear();
			_other.pRawScene        = nullptr;  // 移動させた元が二重解放しないようnullptrに
			_other.playingAnimIndex = 0;
			_other.currentAnimTime  = 0.0;
		}

		// ムーブを許可
		ModelResource& operator=(
		    ModelResource&& _other) noexcept
		{
			if (this != &_other)
			{
				// ベイク済みデータを先に解放してから元シーンを解放する。
				for (ufbx_baked_anim* baked : bakedAnimations)
					if (baked)
						ufbx_free_baked_anim(baked);
				bakedAnimations.clear();
				if (pRawScene)
					ufbx_free_scene(pRawScene);
				pRawScene = nullptr;

				file                = std::move(_other.file);
				subParts            = std::move(_other.subParts);
				isSkinnedModel      = _other.isSkinnedModel;
				bones               = std::move(_other.bones);
				animations          = std::move(_other.animations);
				playingAnimIndex    = _other.playingAnimIndex;
				currentAnimTime     = _other.currentAnimTime;
				bakedAnimations     = std::move(_other.bakedAnimations);
				pRawScene           = _other.pRawScene;
				pBoneConstantBuffer = std::move(_other.pBoneConstantBuffer);

				_other.bakedAnimations.clear();
				_other.pRawScene        = nullptr;
				_other.playingAnimIndex = 0;
				_other.currentAnimTime  = 0.0;
			}
			return *this;
		}

		~ModelResource()
		{
			for (ufbx_baked_anim* baked : bakedAnimations)
				if (baked)
					ufbx_free_baked_anim(baked);
			bakedAnimations.clear();

			if (pRawScene)  // ちゃんと解放
			{
				ufbx_free_scene(pRawScene);
				pRawScene = nullptr;
			}
		}

		inline HashValue GetHash() const
		{
			HashValue hash{};
			HashCombine(hash, file.string());
			return hash;
		}

		/// @brief アニメーション更新描画処理 NOTE: キーフレームではない場合 UpdateAnim を使ってね
		/// @param _deltaTime デルタタイム
		/// @param _animIndex アニメーションインデックス
		void Draw(const float _deltaTime, const size_t _animIndex);

		/// @brief キーフレームではないアニメーションの適用 NOTE: キーフレームの場合 Draw(float, size_t) を使ってね
		/// @param _deltaTime アニメーション時間
		/// @param _animIndex アニメーションインデックス
		void UpdateAnim(const float _deltaTime, const int _animIndex);

		/// @brief 現在のアニメーション時間を設定する
		/// @param _time 現在のアニメーション時間
		void SetCurrentAnimTime(const float _time);

		fs::path file{};  ///< モデルファイル

		std::vector<ModelParts> subParts{};

		// アニメーション対応用に拡張
		bool isSkinnedModel = false;
		std::vector<BoneInfo> bones{};
		std::vector<AnimationClip> animations{};

		size_t playingAnimIndex{ 0 };                     // 再生中のアニメーションインデックス
		double currentAnimTime{ 0.0 };                    // アニメーションの再生時間
		std::vector<ufbx_baked_anim*> bakedAnimations{};  // クォータニオン補間用のベイク済みアニメーション

		ufbx_scene* pRawScene{ nullptr };  // シーンの情報をそのままもっちゃう

		// 各モデル個別のボーン行列をGPUへ送るための定数バッファを追加！
		ComPtr<ID3D11Buffer> pBoneConstantBuffer{ nullptr };
	};

	/// @brief モデルリソースを格納
	class ModelRegistry : public ResourceRegistryBase<ModelResource>
	{
	  public:
		ModelRegistry()  = default;
		~ModelRegistry() = default;

		bool TryLoad(fs::path&& _file, ModelResource& _out) override;

	  private:
		/// @brief FBXモデルを試しにロードする
		/// @param _file fbxフィアルパス
		/// @param _out 出力先
		/// @return ロードに成功 true / false
		bool TryLoadFbx(const fs::path& _file, ModelResource& _out);

		bool TryLoadParts(ufbx_mesh* _pMesh, ufbx_mesh_part* _pPart, ModelParts& _out) const;

		bool TryCreateVertexBuffer(ComPtr<ID3D11Buffer>& _p, const std::vector<ModelVertex> _vertices) const;
		bool TryCreateIndexBuffer(ComPtr<ID3D11Buffer>& _p, const std::vector<uint32_t> _indices) const;

		// ボーン専用の定数バッファを作成するヘルパーを追加
		bool TryCreateBoneBuffer(ComPtr<ID3D11Buffer>& _p) const;
	};

	template <> struct ResourceTraits<ResourceType::Model>
	{
		using DataT       = ModelResource;
		using RegistryT   = ModelRegistry;
		using LoadConfigT = fs::path;
	};
}  // namespace cha
