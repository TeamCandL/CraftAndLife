#include "OtherPlayer.h"
#include <Game/Manager/SessionManager.h>
#include <Game/Manager/SessionManager/SessionStateReader.h>
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

#if 0
		SessionStateReader reader(sessionManager);
		//StateTarget target = StateTarget::Player(name.content);

		// プレイヤー名からIDを取得
		int32_t playerId = -1;

		if (sessionManager.TryGetPlayerId(name.content, playerId))	// 志賀君が言ってたpStatesが0なのと途中から呼ばれていない
		{
			// IDで対象プレイヤーを指定
			StateTarget target = StateTarget::Player(playerId);

			if (reader.TryGetState(target, "hp", Data().hp))
			{
				VCU_LOG("OtherPlayer HP: {}", Data().hp);
			}
			else
			{
				VCU_LOG("HP取得失敗 ID: {}", playerId);//そのためデータは言ってないからずっとここ
			}

			bool flag = reader.TryGetState(target, "aliveFlag", Data().aliveFlag);
			if (flag == 0) // この処理
			{
				if (healthState == HelthState_Dead)	// これ自体は呼ばれていた
				{
					isDead = true; // ホストが死んだときこれ呼ばれなかった
				}
				else
				{
					isDead = false;
				}
			}
			else
			{
				VCU_LOG("helthState取得失敗 ID: {}", playerId); // ここが呼ばれてる
			}
		}
		else
		{
			VCU_LOG("Player ID取得失敗: {}", name.content);
		}

#elif 1

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
#endif

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
