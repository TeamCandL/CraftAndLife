#pragma once
#include "SessionManager/InSessionStatus.h"
#include <Base/Base.h>
#include <Game/Common/Utility/SizedBuffer.h>

/// @brief 部屋に入ってからのプレイヤーとしてのセッションを管理する
class SessionManager : public IManager
{
public:
	SessionManager() = default;
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

private:
	InSessionStatus inSessionStatus_{}; ///< セッション内のステータス
};

template <typename Func> inline void SessionManager::ForeachPlayerStatus(Func&& _func)
{
	for (const auto& [name, status] : inSessionStatus_.playersStatus)
	{
		_func(name, status);
	}
}
