#pragma once
#include <Game/Game.h>

enum struct ProtocolType : uint8_t
{
	JoinRoom = 0,  ///< 部屋を作成・参加 (部屋名)
	JoinOK,        ///< 部屋の参加ok (プレイヤー名)
	JoinNG,
	LobbyMembers,         ///< ロビー内メンバー一覧
	ReadySelf,            ///< ロビー内で準備ok
	StartGame,            ///< ゲーム開始
	StartedGame,          ///< ゲームが開始された
	UpdatePlayer,         ///< プレイヤーの情報更新
	UpdatedPlayers,       ///< 全プレイヤーの更新情報
	UpdatedPlayerStates,  ///< 全プレイヤーのステート情報更新
	UpdateEnemy,          ///< 敵の情報更新
	UpdatedEnemies,       ///< すべての敵の情報更新
	UpdateEnemyDamage,    ///< ゲストからアザーエネミーを殴った時のダメージ情報の更新　
	UpdatedEnemyStates,   ///< 敵のステートの情報を更新
	UpdateEntitySpawn,    ///< エンティティの生成
	UpdateEntityRemove,   ///< エンティティの削除
	UpdateDayTime,        ///< ゲーム内時間の更新要求
	UpdatedDayTime,       ///< ゲーム内時間の同期情報
	LoadedTerrain,        ///< 地形のチャンク情報を取得
	FinishGame,           ///< ゲームが終わった
	ResultInfo,           ///< 結果情報
	LeaveRoom,            ///< 部屋から離脱
	FireEnemy,            ///< 敵が攻撃っした

	BreakHarvestable,  ///< ハーベスタブルオブジェクトを破壊

	EntitySpawnRequest,  ///< 指定のアイテムエンティティを出現させたい！と伝える
	EntitySpawned,       ///< アイテムエンティティを出現させる
	EntityPickRequest,   ///< 指定のアイテムエンティティを拾いたい！と伝える
	EntityPicked,        ///< 指定したエンティティは拾われた

	PlayerRespawned,  ///< プレイヤー蘇生の通信

	TextChated,

	Idk,
};
