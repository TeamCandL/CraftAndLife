#pragma once
#include <Game/Game.h>
#include "EnemyStatus.h"
#include <Game/PlayScene/NetworkActorServer.h>

struct EnemyControllerData
{
	EnemyControllerData(
	    GameObject _objPlaySceneUIController) :
	    objPlaySceneUIController{ _objPlaySceneUIController }
	{
	}

	std::vector<GameObject> allEnemies;       ///< 全ての敵の情報を格納する
	std::vector<GameObject> allAliveEnemies;  ///<	全ての生きている敵を格納する
	int32_t nextEnemyId = 0;

	GameObject objBossInfo{};               ///< ボスのHPUIのオブジェクト
	GameObject objPlaySceneUIController{};  ///< UIコントローラーのオブジェクト
	GameObject objNetworkActorServer{};     ///< ネットワークアクターサーバーのオブジェクト

	GameObject objEnemiesInfo{};  // 配列にする必要あるかもね

	std::vector<GameObject> objEnemyHpBar;
};

struct EnemyController : ObjectBase<EnemyControllerData>
{
	using ObjectBase<EnemyControllerData>::ObjectBase;
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;

	void AddEnemy(const GameObject _enemy);

	/// @brief 全ての敵の座標を取得する
	/// @return 座標一覧を返す
	std::vector<Vec3> GetAllEnemyPosition();
	std::vector<float> GetAllEnemyAngleY();
	std::vector<int32_t> GetAllEnemyId();
	std::vector<EnemyStatus> GetAllEnemyStatus();
	std::vector<GameObject> GetAllEnemies();
	std::vector<GameObject> GetAllAliveEnemies();

	/// @brief IDを指定して敵にダメージを与える
	/// @param enemyId ダメージを受ける敵のID
	/// @param damage ダメージ量
	void DamagedEnemy(int32_t enemyId, int32_t damage);

	/// @brief IDを指定して敵の管理スロットを空にする
	void RemoveEnemy(int32_t enemyId);

	/// @brief 指定したIDの敵が存在するか確認
	bool IsExistEnemy(int32_t enemyId);

	void SetNetworkActorServer(GameObject obj);

	GameObject GetNetworkActorServer() const;

	void DeadEnemy(GameObject _objEnemy);
	void PopEnemy(int32_t _id);
};