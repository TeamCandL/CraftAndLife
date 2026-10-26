#pragma once
#include "../../BasePch.h"
#include "../ResourceTraits.h"
#include "IResourceRegistry.h"

namespace cha
{
	/// @brief 有効なリソース設定構造体 fs::pathまたはHashValueを返すメンバ変数GetHash()constを許可する概念
	template <typename T>
	concept ValidResourceLoadConfig = std::is_same_v<T, fs::path> || requires(const T& config) {
		{ config.GetHash() } -> std::same_as<HashValue>;
	};

	/// @brief リソースレジストリのベース
	/// @tparam ResourceDataT リソースデータの構造体型
	/// @tparam ResourceLoadConfigT リソース読み込む設定型 (デフォルトでファイルパス)
	template <typename ResourceDataT, typename ResourceLoadConfigT = fs::path>
	class ResourceRegistryBase : public IResourceRegistry
	{
	  public:
		using DataT       = ResourceDataT;
		using LoadConfigT = ResourceLoadConfigT;

		// ResourceLoadConfigT が const で GetHash() を呼べ、
		// 戻り値が HashValue であることを検証
		static_assert(
		    ValidResourceLoadConfig<LoadConfigT>,
		    "リソースの読み込み設定構造体は必ずメンバ関数 'HashValue GetHash() const' を持っている必要があります。");

	  private:
		struct Slot
		{
			ResourceDataT data{};
			uint32_t generation{ 1 };  /// 世代番号 1から開始
			bool isAlive{ false };     ///< 使用中フラグ
		};

		// ビットマスク用の定数
		static constexpr uint32_t INDEX_BITS = 20;
		static constexpr uint32_t GEN_BITS   = 12;
		static constexpr uint32_t INDEX_MASK = (1u << INDEX_BITS) - 1u;  // 0x000FFFFF
		static constexpr uint32_t GEN_MASK   = (1u << GEN_BITS) - 1u;    // 0x00000FFF

	  public:
		ResourceRegistryBase();
		virtual ~ResourceRegistryBase() = default;

		// リソースプールをクリアする（安全に破棄するため）
		inline void Clear()
		{
			registry_.clear();
			pathIndex_.clear();
			succededLast_ = true;
			// 最低1つはスロットを確保しておく従来の挙動を維持
			registry_.emplace_back();
		}

		/// @brief リソースを読み込む or 取得する
		/// @param _config 設定
		/// @return ハンドル (失敗時無効ハンドル)
		inline virtual Handle AddOrGet(
		    ResourceLoadConfigT&& _config) final
		{
			// 重複読み込み防止: LoadConfig が fs::path の場合は既存の登録を検索してハンドルを返す
			if constexpr (std::is_same_v<LoadConfigT, fs::path>)
			{
				const std::string key = fs::path{ _config }.lexically_normal().string();
				auto itr              = pathIndex_.find(key);
				if (itr != pathIndex_.end())
				{
					return itr->second;
				}
			}
			else  // 重複読み込み防止: fs::pathではないならハッシュ値から重複検出
			{
				const HashValue key = _config.GetHash();
				if (key != 0)  // かつハッシュ値が0でないなら。
				{
					auto itr = hashIndex_.find(key);
					if (itr != hashIndex_.end())
					{
						return itr->second;
					}
				}
			}

			if (succededLast_)
			{
				// 前回成功していたなら新規作成
				registry_.push_back({});
			}
			else
			{
				// 失敗していたなら使い回す
				registry_.back() = std::move(ResourceDataT{});
			}
			ResourceDataT& data = registry_.back();

			ResourceLoadConfigT config{ std::move(_config) };
			if constexpr (std::is_same_v<LoadConfigT, fs::path>)
			{
				fs::path configPath{};
				configPath = config;
				if (succededLast_ = TryLoad(std::move(config), data))
				{
					// 読み込み成功で作られるハンドル
					const Handle CREATED_HANDLE{ static_cast<Handle>(registry_.size() - 1) };

					const std::string key = configPath.lexically_normal().string();
					pathIndex_[key]       = CREATED_HANDLE;
					return CREATED_HANDLE;
				}
				VCU_LOG("新しく読み込んだが失敗したリソースファイルのパス:{}", configPath.string());
			}
			else
			{
				HashValue hashValue = config.GetHash();
				if (succededLast_ = TryLoad(std::move(config), data))
				{
					// 読み込み成功で作られるハンドル
					const Handle CREATED_HANDLE{ static_cast<Handle>(registry_.size() - 1) };

					hashIndex_[hashValue] = CREATED_HANDLE;
					return CREATED_HANDLE;
				}
				VCU_LOG("新しく読み込んだが失敗したリソースファイルのハッシュ値:{}", hashValue);
			}
			return INVALID_HANDLE;
		}

		/// @brief リソースを追加する
		/// @param _data リソースデータ構造体
		/// @return ハンドル
		inline Handle Add(
		    ResourceDataT&& _data)
		{
			if (succededLast_)
			{
				// 前回成功していたなら新規作成
				registry_.emplace_back(std::move(_data));
			}
			else
			{
				// 失敗していたなら使い回す
				registry_.back() = std::move(_data);
			}
			succededLast_ = true;
			return static_cast<Handle>(registry_.size() - 1);
		}

		/// @brief リソースを取得する
		/// @param _handle ハンドル
		/// @return リソースの参照ポインタ
		inline ResourceDataT& GetResource(
		    const Handle _handle)
		{
			return registry_[_handle];
		}

	  protected:
		/// @brief 試しに読み込み設定からリソースをロードする
		/// @param _config 読み込み設定
		/// @param _out ロード出力先
		/// @return ロードに成功 true / false
		inline virtual bool TryLoad(
		    ResourceLoadConfigT&& _config,
		    ResourceDataT& _out)
		{
			return false;
		}

	  private:
		// 前回読み込みに成功したか
		bool succededLast_{ true };
		// リソースのプール
		std::vector<ResourceDataT> registry_{};

		// ファイルパスベースのリソースに対する重複検出用インデックス
		// LoadConfigT が fs::path のときのみ利用する
		std::unordered_map<std::string, Handle> pathIndex_{};
		std::unordered_map<HashValue, Handle>
		    hashIndex_{};  ///< ハッシュ値ベースのリソース重複検出用ルックアップテーブル
	};
	template <typename ResourceDataT, typename ResourceLoadConfigT>
	inline ResourceRegistryBase<ResourceDataT, ResourceLoadConfigT>::ResourceRegistryBase()
	{
		registry_.emplace_back();
	}
}  // namespace cha
