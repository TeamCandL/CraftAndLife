#pragma once
#include <string>
#include <vector>
#include <initializer_list>
#include <variant>

/// @brief ステートの値
using StateValue = std::variant<bool, uint8_t, int32_t, float>;

/// @brief 監視対象のポインタ
using StatePointer = std::variant<const bool*, const uint8_t*, const int32_t*, const float*>;

/// @brief ステート変更情報
struct StateChange
{
	std::string name{};  ///< 変更されたステートの名前
	StateValue value{};  ///< 変更後のステートの値
};

/// @brief 登録されたステート情報
struct RegisteredState
{
	std::string name{};          ///< 登録されたステートの名前
	StatePointer value{};        ///< 監視対象のbool変数へのポインタ
	StateValue previousValue{};  ///< 前回確認した時のステートの値
};

/// @brief 複数登録用のステート情報
struct StateEntry
{
	const char* name;
	StatePointer value;

	template <typename T>
	StateEntry(
	    const char* name,
	    const T& value) :
	    name(name),
	    value(&value)
	{
	}
};

/// @brief 敵のステートを登録・監視するシステム
struct StateRegistry
{
	std::vector<RegisteredState> states;  ///< 登録されたステートの一覧

	/// @brief ステートを登録する
	template <typename T>
	void Register(
	    const std::string& name,
	    const T& value)
	{
		for (auto& state : states)
		{
			if (state.name == name)
			{
				state.value         = &value;
				state.previousValue = value;
				return;
			}
		}

		states.push_back({ name, &value, value });
	}

	/// @brief 複数のbool変数をまとめて登録する
	/// @param entries ステート名とbool変数の参照の一覧
	void Register(std::initializer_list<StateEntry> entries);

	/// @brief キューへの登録が成功した変更を処理済みにする
	/// @param changes 処理済みにするステートの一覧
	void CommitChanges(const std::vector<StateChange>& changes);

	/// @brief 前回処理済みの値から変化したステートを取得する
	/// @return 変更されたステートの一覧（前回値は更新しない）
	std::vector<StateChange> GetChanges() const;

	/// @brief 登録されている全ステートの現在値を取得する
	/// @return 全ステートの名前と現在値の一覧
	std::vector<StateChange> GetAllStates() const;

	/// @brief 全取得と差分取得を切り替える
	std::vector<StateChange> GetStates(bool fullSync) const;

	/// @brief 指定したステートの登録を解除する
	void Unregister(const std::string& name);

	/// @brief 登録されている全ステートを解除する
	void Clear();
};
