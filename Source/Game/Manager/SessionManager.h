#pragma once
#include "SessionManager/InSessionStatus.h"
#include <Base/Base.h>
#include <Game/Common/Utility/SizedBuffer.h>

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

	/// @brief ほかメンバーの名前一覧をセットする
	/// @param _names ほかプレイヤーの名前一覧
	void SetMemberNames(const std::string_view _names);

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
	void UpdatePlayers(const std::string_view _names, SizedBuffer<float>& _positions, SizedBuffer<float>& _rotations);

	/// @brief プレイヤーステータスを参照する
	/// @param _playerName プレイヤー名
	/// @return プレイヤーステータスの参照
	const InSessionStatus::PlayerStatus& RefPlayerStatus(const std::string& _playerName) const;

	/// @brief 指定のプレイヤーが存在しているかを取得
	/// @param _playerName 指定のプレイヤー名
	/// @return 存在している true / false
	bool IsExistPlayer(const std::string& _playerName) const;

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
	    SizedBuffer<int>& _enemyIds,
	    SizedBuffer<int>& _enemyTypes,
	    SizedBuffer<float>& _positions,
	    SizedBuffer<float>& _rotations);

	/// @brief 指定した敵が存在するか
	/// @param _enemyId ステージに存在してる敵のID
	/// @return 存在している true / false
	bool IsExistEnemy(int _enemyId) const;

	/// @brief 各敵を周回する
	/// @tparam Func コールバック [](int, const InSessionStatus::EnemyStatus&){}
	/// @param _func コールバック [](int, const InSessionStatus::EnemyStatus&){}
	template <typename Func> void ForeachEnemyStatus(Func&& _func);

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
	void UpdateDayTime(float _currentTimeSec);

  private:
	InSessionStatus inSessionStatus_{};  ///< セッション内のステータス
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