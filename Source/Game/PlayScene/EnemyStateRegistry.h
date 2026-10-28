#pragma once
#include <string>
#include <vector>
#include <functional>
#include <initializer_list>
#include <utility>

/// @brief 敵のステート変更情報
struct EnemyStateChange
{
	std::string name{};  ///< 変更されたステートの名前
	bool value{};        ///< 変更後のステートの値
};

/// @brief 登録された敵ステート情報
struct RegisteredEnemyState
{
	std::string name{};           ///< 登録されたステートの名前
	const bool* value = nullptr;  ///< 監視対象のbool変数へのポインタ
	bool previousValue{};         ///< 前回確認した時のステートの値
};

/// @brief 敵のステートを登録・監視するシステム
struct EnemyStateRegistry
{
	std::vector<RegisteredEnemyState> states;  ///< 登録されたステートの一覧

	/// @brief 敵のbool変数を監視対象として登録する
	/// @param name 登録するステートの名前
	/// @param value 監視対象となるbool変数
	void Register(const std::string& name, const bool& value);

	/// @brief 複数のbool変数をまとめて登録する
	/// @param entries ステート名とbool変数の参照の一覧
	void Register(std::initializer_list<std::pair<std::string, std::reference_wrapper<const bool>>> entries);

	/// @brief 前回処理済みの値から変化したステートを取得する
	/// @return 変更されたステートの一覧（前回値は更新しない）
	std::vector<EnemyStateChange> GetChanges() const;

	/// @brief キューへの登録が成功した変更を処理済みにする
	/// @param changes 処理済みにするステートの一覧
	void CommitChanges(const std::vector<EnemyStateChange>& changes);

	/// @brief 登録されている全ステートの現在値を取得する
	/// @return 全ステートの名前と現在値の一覧
	std::vector<EnemyStateChange> GetAllStates() const;

	/// @brief 登録されている全ステートを解除する
	void Clear();
};