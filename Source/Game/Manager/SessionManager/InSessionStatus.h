#pragma once
#include <Game/Game.h>

/// @brief セッション内のステータス
struct InSessionStatus
{
	/// @brief プレイヤーのステータス
	struct PlayerStatus
	{
		Vec3 position{ 0, 10, 0 };  ///< 座標
		Vec2 rotation{ 0, 0 };      ///< 角度データ
		bool updatedFlag{ false };  ///< 更新時のフラグ false / true 切り替え用
	};

	struct EnemyStatus
	{
		int enemyType{ 0 };         ///< 種類
		Vec3 position{ 0, 10, 0 };  ///< 座標
		Vec2 rotation{ 0, 0 };      ///< 角度データ
		bool updatedFlag{ false };  ///< 更新時のフラグ false / true 切り替え用

		std::unordered_map<std::string, bool> states{};
	};

	std::string joinRoomName{};              ///< 参加する部屋名
	std::string name{};                      ///< プレイヤー名
	std::vector<std::string> memberNames{};  ///< ロビー内メンバー名一覧

	bool imLobbyHost{ false };                                      ///< 私はホストであるか
	bool updatedFlag{ false };                                      ///< 更新確認用フラグ false / true 切り替え用
	std::unordered_map<std::string, PlayerStatus> playersStatus{};  ///< プレイヤーの座標情報
	std::unordered_map<int, EnemyStatus> enemiesStatus{};           ///< 敵の座標情報

	float currentTimeSec;              ///< 昼夜サイクルの現在時刻
	bool hasReceivedDayTime{ false };  ///< 昼夜時刻を受信済みか
};
