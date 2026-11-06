#include "OtherPlayer.h"
#include <Game/Manager/SessionManager.h>
#include "Player/HelthState.h"
#include "OtherPlayerHand.h"

void OtherPlayer::Start() {}

void OtherPlayer::Update()
{
	SessionManager& sessionManager = Get<SessionManager>();

	if (!sessionManager.HasAllPlayersStatus())
	{
		return;
	}

	if (sessionManager.IsExistPlayer(name.content))
	{
		const InSessionStatus::PlayerStatus& status = sessionManager.RefPlayerStatus(name.content);

		// =====================================
		// HPを取得
		// =====================================

		auto hpIt = status.states.find("hp");

		if (hpIt != status.states.end())
		{
			if (const int32_t* receivedHP = std::get_if<int32_t>(&hpIt->second))
			{
				Data().hp = *receivedHP;
			}
		}

		// =====================================
		// 死亡状態を取得
		// =====================================

		isDead = false;

		auto stateIt = status.states.find("helthState");

		if (stateIt != status.states.end())
		{
			if (const int32_t* healthState = std::get_if<int32_t>(&stateIt->second))
			{
				isDead = (*healthState == HelthState_Dead);
			}
		}

		// =====================================
		// 座標と角度を更新
		// =====================================

		transform.position = status.position;

		transform.rotation = Quaternion::FromEuler(Vec3{ 0.0f, status.rotation.y, 0.0f });

		VCU_LOG(
		    "{}の更新！座標:({}, {}, {}) 回転y:{}",
		    name.content,
		    transform.position.x,
		    transform.position.y,
		    transform.position.z,
		    transform.rotation.y);
	}
	else
	{
		VCU_LOG("{}がない！", name.content);
	}
}

void OtherPlayer::Draw()
{
	transform.UpdateFamily();

	Get<Dx11Manager>().SetShader(shader);
	Get<Dx11Manager>().SetMaterial(transform.GetMaterial());

	models[role].Get().Draw(Get<DeltaTime>(), animIndex);

	const auto& subParts{ models[role].Get().subParts };

	for (const auto& parts : subParts)
	{
		Get<Dx11Manager>().SetSurface(parts.mesh);
		Get<Dx11Manager>().SetMaterial(parts.material);
		Get<Dx11Manager>().SetMaterial(parts.transformMaterial);
		Get<Dx11Manager>().Draw();
	}
}

Vec3 OtherPlayer::GetPosition() const
{
	return transform.position;
}

float OtherPlayer::GetAngleY() const
{
	return Vec3{ transform.rotation.ToEuler() }.y;
}
