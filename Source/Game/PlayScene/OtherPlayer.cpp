#include "OtherPlayer.h"
#include <Game/Manager/SessionManager.h>
#include <Game/Manager/SessionManager/SessionStateReader.h>
#include "Player/HelthState.h"
#include "OtherPlayerHand.h"
#include "Player/PlayerAnimType.h"

void OtherPlayer::Start()
{
	const auto& status = Get<SessionManager>().RefStatus();

	// プレイヤー名からIDを検索
	auto it = std::ranges::find(status.memberNames, name.content);

	if (it == status.memberNames.end())
	{
		VCU_LOG("プレイヤーが見つかりません: {}", name.content);
		return;
	}

	// 名前とIDは同じインデックスで管理されている
	const size_t index = std::distance(status.memberNames.begin(), it);

	if (index >= status.memberId.size())
	{
		return;
	}

	const int32_t playerId = status.memberId[index];

	// IDからRoleIdを取得
	auto roleIt = status.memberRoles.find(playerId);

	if (roleIt == status.memberRoles.end())
	{
		VCU_LOG("RoleId取得失敗: {}", playerId);
		return;
	}

	role = roleIt->second;

	VCU_LOG("{} のRoleId: {}", name.content, role);
}

void OtherPlayer::Update()
{
	SessionManager& sessionManager = Get<SessionManager>();

	if (!sessionManager.HasAllPlayersStatus())
	{
		return;
	}

	if (!sessionManager.IsExistPlayer(name.content))
	{
		VCU_LOG("{}がない！", name.content);
		return;
	}

	const InSessionStatus::PlayerStatus& status = sessionManager.RefPlayerStatus(name.content);

	const InSessionStatus& dstatus = Get<SessionManager>().RefStatus();

	VCU_LOG("日付：{}", dstatus.dayCount);
	VCU_LOG("時間：{}", dstatus.currentTimeSec);
	VCU_LOG("受信済み：{}", dstatus.hasReceivedDayTime);
	if (dstatus.dayCount == 2)
	{
		VCU_LOG("2日目ー！");
	}

	// =====================================
	// プレイヤーステートを取得
	// =====================================

	SessionStateReader reader(sessionManager);

	int32_t hp = 0;

	const int32_t enemyId     = 0;
	const StateTarget etarget = StateTarget::Enemy(enemyId);

	if (reader.TryGetState(etarget, "hp", hp))
	{
		VCU_LOG("敵 ID={} のHP: {}", enemyId, hp);
	}
	else
	{
		VCU_LOG("敵 ID={} のHP取得失敗", enemyId);
	}

	// プレイヤー名からIDを取得
	int32_t playerId = -1;

	if (!sessionManager.TryGetPlayerId(name.content, playerId))
	{
		VCU_LOG("Player ID取得失敗: {}", name.content);
		return;
	}

	// 対象プレイヤーを指定
	const StateTarget target = StateTarget::Player(playerId);

	// =====================================
	// HPを取得
	// =====================================

	if (reader.TryGetState(target, "hp", Data().hp))
	{
		VCU_LOG("OtherPlayer HP: {}", Data().hp);
	}
	else
	{
		VCU_LOG("HP取得失敗 ID: {}", playerId);
	}

	// =====================================
	// 生死状態を取得
	// =====================================

	int32_t healthState = HelthState_Live;

	if (reader.TryGetState(target, "helthState", healthState))
	{
		isDead = (healthState == HelthState_Dead);

		VCU_LOG("OtherPlayer 生死状態取得成功: ID={}, State={}, Dead={}", playerId, healthState, isDead);
	}
	else
	{
		VCU_LOG("helthState取得失敗 ID: {}", playerId);
	}

	// =====================================
	// アニメーション番号を取得
	// =====================================

	if (reader.TryGetState(target, "animIndex", animIndex))
	{
		// 取得成功
	}
	else
	{
		VCU_LOG("animIndex取得失敗 ID: {}", playerId);
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

	if (!isDead)
	{
		reader.TryGetState(target, "animIndex", animIndex);
	}

	// =====================================
	// 死亡アニメーション
	// =====================================
	if (isDead)
	{
		animIndex = PlayerAnimType_Die;

		if (dieAnimTimeSec > 0.0f)
		{
			dieAnimTimeSec = std::max(0.0f, dieAnimTimeSec - Get<DeltaTime>());

			animSpeed = 1.0f;
		}
		else
		{
			animSpeed = 0.0f;
		}
		// animSpeed = 0.0f;
	}
}

void OtherPlayer::Draw()
{
	transform.UpdateFamily();

	Get<Dx11Manager>().SetShader(shader);
	Get<Dx11Manager>().SetMaterial(transform.GetMaterial());

	models[role].Get().Draw(Get<DeltaTime>() * animSpeed, animIndex);

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
