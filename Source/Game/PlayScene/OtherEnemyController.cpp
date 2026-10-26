#include "OtherEnemyController.h"
#include "OtherEnemy.h"
#include <Game/Manager/SessionManager.h>

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
		    }
	    });
}
