#pragma once
#include "SessionManager/InSessionStatus.h"
#include <Base/Base.h>
#include <Game/Common/Utility/SizedBuffer.h>

struct StateRegistry;

/// @brief 部屋に入ってからのプレイヤーとしてのセッションを管理する
class SessionManager : public IManager
{
  public:
	SessionManager()  = default;
	~SessionManager() = default;

	/// @brief セッション情報をリセットする
	void Reset();

	/// @brief プレイヤー名をセットする
	/// @param _name 名前
	void SetPlayerName(const std::string_view _name);

	/// @brief プレイヤーの役割を設定する
	/// @param _playerId プレイヤーID
	/// @param _roleId 役割ID
	void SetPlayerRole(int32_t _playerId, int32_t _roleId);

	/// @brief プレイヤーの役割を取得する
	/// @param _playerId プレイヤーID
	/// @param _outRoleId 取得した役割ID
	/// @return 取得成功 true / false
	bool TryGetPlayerRole(int32_t _playerId, int32_t& _outRoleId) const;

	/// @brief ほかメンバーの名前一覧をセットする
	/// @param _names ほかプレイヤーの名前一覧
	void SetMemberInfo(const std::string_view _members);

	/// @brief 参加する部屋名をセットする
	/// @param _name 部屋名
	void SetJoinRoomName(const std::string_view _name);

	/// @brief 自分がロビーのホストかどうかの設定をする
	/// @pram _isLobbyHost　ホストかどうか
	void SetLobbyHost(bool _isLobbyHost);

	/// @brief ワールド内の全プレイヤーを更新する
	/// @param _names プレイヤー名
	/// @param _positions 座標情報
	/// @param _rotations 回転情報
	void UpdatePlayers(
	    const std::string_view _names,
	    SizedBuffer<float>& _positions,
	    SizedBuffer<float>& _rotations,
	    SizedBuffer<int32_t>& _haveItemIds,
	    SizedBuffer<int32_t>& _attentionLevel);

	/// @brief プレイヤーステータスを参照する
	/// @param _playerName プレイヤー名
	/// @return プレイヤーステータスの参照
	const InSessionStatus::PlayerStatus& RefPlayerStatus(const std::string& _playerName) const;

	/// @brief 指定のプレイヤーが存在しているかを取得
	/// @param _playerName 指定のプレイヤー名
	/// @return 存在している true / false
	bool IsExistPlayer(const std::string& _playerName) const;

	// プレイヤーIDからプレイヤー名を取得
	bool TryGetPlayerName(int32_t _playerId, std::string& _outName) const;

	// プレイヤー名からプレイヤーIDを取得
	bool TryGetPlayerId(std::string_view _playerName, int32_t& _outId) const;

	/// @brief 自分も含め全プレイヤーのステータスが存在するか
	/// @return 存在する true / false
	bool HasAllPlayersStatus() const;

	/// @brief 受信した複数のプレイヤーステートを更新する
	/// @param _playerId プレイヤーID
	/// @param _stateNames 改行区切りのステート名一覧
	/// @param _values 各ステートの値
	/// @return 1件以上のステートが追加・変更された場合 true
	bool UpdatePlayerStates(int32_t _playerId, std::string_view _stateNames, SizedBuffer<uint8_t>& _values);

	/// @brief 各プレイヤーを周回する
	/// @tparam Func コールバック [](const std::string&, const InSessionStatus::PlayerStatus&){}
	/// @param _func コールバック [](const std::string&, const InSessionStatus::PlayerStatus&){}
	template <typename Func> void ForeachPlayerStatus(Func&& _func);

	/// @brief inSessionStatusを参照する
	/// @return inSessionStatus_のconst参照ポインタ
	const InSessionStatus& RefStatus() const
	{
		return inSessionStatus_;
	}

	/// @brief ワールド内の全敵を更新する
	/// @param _enemyIds   ステージに存在してる敵のID
	/// @param _ememyTypes 敵の種類
	/// @param _positions  座標情報
	/// @param _rotations  回転情報
	void UpdateEnemies(
	    SizedBuffer<int32_t>& _enemyIds,
	    SizedBuffer<int32_t>& _enemyTypes,
	    SizedBuffer<float>& _positions,
	    SizedBuffer<float>& _rotations);

	/// @brief 指定した敵が存在するか
	/// @param _enemyId ステージに存在してる敵のID
	/// @return 存在している true / false
	bool IsExistEnemy(int _enemyId) const;

	/// @brief 受信した複数の敵ステートを更新する
	/// @param _enemyIds 敵ID一覧
	/// @param _stateNames 改行区切りのステート名一覧
	/// @param _values 各ステートの値
	/// @return 1件以上のステートが追加・変更された場合 true
	bool UpdateEnemyStates(
	    SizedBuffer<int32_t>& _enemyIds,
	    std::string_view _stateNames,
	    SizedBuffer<uint8_t>& _values);

	bool TryGetEnemyAlive(int32_t _enemyID, bool& _outAlive) const;

	/// @brief 各敵を周回する
	/// @tparam Func コールバック [](int, const InSessionStatus::EnemyStatus&){}
	/// @param _func コールバック [](int, const InSessionStatus::EnemyStatus&){}
	template <typename Func> void ForeachEnemyStatus(Func&& _func);

	// ローカルプレイヤーのステートレジストリを登録
	void SetSelfStateRegistry(const StateRegistry* _registry);

	// ローカルプレイヤーのステートレジストリを取得
	const StateRegistry* GetSelfStateRegistry() const;

	/// @brief 敵IDを参照する
	/// @param _enemyId ステージに存在してる敵のID
	/// @return 敵のステータスの参照
	const InSessionStatus::EnemyStatus& RefEnemyStatus(
	    int _enemyId) const
	{
		return inSessionStatus_.enemiesStatus.at(_enemyId);
	}

	/// @brief 昼夜サイクルのためのゲーム内時間の更新
	/// @param _currentTimeSec ホスト側の昼夜サイクルの現在時刻
	void UpdateDayTime(float _currentTimeSec, int32_t _dayCount);

	int GetEnemyStateSize()
	{
		return inSessionStatus_.enemiesStatus.size();
	}

  private:
	InSessionStatus inSessionStatus_{};  ///< セッション内のステータス
	const StateRegistry* selfStateRegistry_{};
};

template <typename Func>
inline void SessionManager::ForeachPlayerStatus(
    Func&& _func)
{
	for (const auto& [name, status] : inSessionStatus_.playersStatus)
	{
		_func(name, status);
	}
}

template <typename Func>
inline void SessionManager::ForeachEnemyStatus(
    Func&& _func)
{
	for (const auto& [id, status] : inSessionStatus_.enemiesStatus)
	{
		_func(id, status);
	}
}