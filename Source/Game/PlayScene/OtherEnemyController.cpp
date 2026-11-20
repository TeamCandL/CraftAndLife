#include "OtherEnemyController.h"
#include "OtherEnemy.h"
#include <Game/Manager/SessionManager.h>
#include "UI/BossInfo.h"
#include "UI/BossHp.h"
#include "UI/BossName.h"
#include "UI/BossGauge.h"
#include "UI/BossGaugeR.h"
#include "OtherEnemyHpBar.h"
#include "Enemy.h"
#include "PlaySceneUIController.h"
#include "UI/MiniMap.h"

#include "../Manager/SessionManager/SessionStateReader.h"

void OtherEnemyController::Start() {}

void OtherEnemyController::Update()
{
	// VCU_LOG("OtherEnemyController Update!");
	SessionManager& sessionManager = Get<SessionManager>();

	// セッション情報から消えた敵のオブジェクトを破棄
	for (auto& [enemyId, obj] : idToObjOtherEnemy)
	{
		if (!sessionManager.IsExistEnemy(enemyId))
		{
			GetScene().Destroy(obj);
		}
	}

	int enemyStateSize = sessionManager.GetEnemyStateSize();
	while (enemySpawnList.size() < enemyStateSize)
	{
		int listSize = enemySpawnList.size();
		enemySpawnList.push_back({ listSize });
		VCU_LOG("呼ばれたぜよ");
	}

	// セッション内の敵情報をもとにほかプレイヤー側の敵を管理
	std::erase_if(
	    idToObjOtherEnemy,
	    [](const auto& idAndObj)
	    {
		    return idAndObj.second.IsEmpty();
	    });

	sessionManager.ForeachEnemyStatus(
	    [this](int enemyId, const InSessionStatus::EnemyStatus& status)
	    {
		    // 敵オブジェクトが未生成なら生成
		    if (idToObjOtherEnemy.count(enemyId) ==
		        0)  // idToObjOtherEnemyに登録されてなければ敵の生成処理をし、idToObjOtherEnemyに登録をする
		    {
			    EnemyType type = ToEnemyType(status.enemyType);

			    VCU_LOG("OtherEnemy生成開始: ID={}", enemyId);

			    if (!enemySpawnList[enemyId].IsSpawned())
			    {
				    GameObject obj = GetScene().Instantiate<OtherEnemy>(enemyId, type);
				    enemySpawnList[enemyId].Spawned();

				    idToObjOtherEnemy[enemyId] = obj;  // 敵IDと紐づけて敵の情報をidToObjOtherEnemyに登録
			    }
		    }
		    else
		    {
			    if (idToObjOtherEnemyHpBar.count(enemyId) !=
			        0)  // 取得した敵IDのHPバーが生成済み(idToObjOtherEnemyHpBarに登録済)ならreturnする
			    {
				    return;
			    }

			    EnemyType type = ToEnemyType(status.enemyType);

			    SessionManager& sessionManager = Get<SessionManager>();
			    SessionStateReader reader(sessionManager);

			    StateTarget target = StateTarget::Enemy(enemyId);
			    int32_t _hp        = 0;
			    if (!reader.TryGetState(target, "hp", _hp))
			    {
				    VCU_LOG("OtherEnemyのHP取得失敗: ID={}", enemyId);
				    return;
			    }

			    // ステータスを経由して敵HPを取得しHPバーを生成
			    InSessionStatus::EnemyStatus enemyStatus = sessionManager.RefEnemyStatus(enemyId);
			    Vec3 _position                           = enemyStatus.position;
			    GameObject obj                           = idToObjOtherEnemy.at(enemyId);
			    GameObject hpBar = GetScene().Instantiate<OtherEnemyHpBar>(obj, _position, _hp, _hp);
			    objOtherEnemyHpBar.push_back(hpBar);
			    idToObjOtherEnemyHpBar[enemyId] = hpBar;  // 生成したHPバーを敵IDと紐づけて登録

			    // ボスなら、ここでBossInfoに対象の敵IDを設定する
			    if (IsBossEnemy(type))
			    {
				    VCU_LOG("ボスOtherEnemy生成: ID={}", enemyId);

				    int32_t maxHP = EnemyParam::Hp[static_cast<int>(type)];

				    PlaySceneUIController& uiController = Get<PlaySceneUIController>(Data().objPlaySceneUIController);

				    Get<BossInfo>(uiController.Data().objBossInfo).SetBoss(enemyId, maxHP);

				    Get<BossHp>(uiController.Data().objBossHp).SetActive(true);
				    Get<BossName>(uiController.Data().objBossName).SetActive(true);
				    Get<BossGauge>(uiController.Data().objBossGauge).SetActive(true);
				    Get<BossGaugeR>(uiController.Data().objBossGaugeR).SetActive(true);
			    }
		    }
	    });
}
