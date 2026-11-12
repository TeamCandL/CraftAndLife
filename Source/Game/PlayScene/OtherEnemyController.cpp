#include "OtherEnemyController.h"
#include "OtherEnemy.h"
#include <Game/Manager/SessionManager.h>
#include "UI/BossInfo.h"
#include "UI/BossHp.h"
#include "UI/BossName.h"
#include "UI/BossGauge.h"
#include "UI/BossGaugeR.h"
#include "EnemyHpBar.h"
#include "Enemy.h"
#include "PlaySceneUIController.h"

namespace
{
	bool IsBossEnemy(
	    EnemyType type)
	{
		return type == EnemyType::BossSkeleton || type == EnemyType::Dragon;
	}
}  // namespace

void OtherEnemyController::Start() {}

void OtherEnemyController::Update()
{
	// VCU_LOG("OtherEnemyController Update!");
	SessionManager& sessionManager = Get<SessionManager>();

	for (auto& [enemyId, obj] : idToObjOtherEnemy)
	{
		if (!sessionManager.IsExistEnemy(enemyId))
		{
			GetScene().Destroy(obj);
		}
	}

	std::erase_if(
	    idToObjOtherEnemy,
	    [](const auto& idAndObj)
	    {
		    return idAndObj.second.IsEmpty();
	    });

	sessionManager.ForeachEnemyStatus(
	    [this](int enemyId, const InSessionStatus::EnemyStatus& status)
	    {
		    if (idToObjOtherEnemy.count(enemyId) == 0)
		    {
			    EnemyType type = ToEnemyType(status.enemyType);

			    VCU_LOG("OtherEnemy生成開始: ID={}", enemyId);

			    GameObject obj = GetScene().Instantiate<OtherEnemy>(enemyId, type);

			    VCU_LOG("生成後のオブジェクトが空？ {}", obj.IsEmpty());

			    idToObjOtherEnemy[enemyId] = obj;

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
