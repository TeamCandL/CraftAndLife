#include "Player.h"
#include "PlayerAttackColl.h"
#include "IMainCamera.h"
#include "Player/PlayerControllerNormal.h"
#include "pch\pch.h"
#include <Game/System/AbstructInputSystem.h>
#include "Entity.h"
#include <Game/PlayScene/TerrainSystem.h>
#include <Game/PlayScene/EntitySystem.h>
#include <Game/Manager/SessionManager.h>
#include <string>
#include "PlayerHand.h"
#include "Player/IPlayerJobController.h"
#include "Player/PlayerJobControllerHealer.h"
#include "Player/PlayerJobControllerTank.h"
#include "Player/PlayerJobControllerFighter.h"
#include "Player/PlayerJobControllerArcher.h"
#include "StateRegistry.h"

void Player::Start()
{
	obb.isTrigger = true;

	transform.scale = Vec3::One() * 200.0f;
	obb.offset      = OBB_OFFSET;
	obb.scale       = Vec3::One() * OBB_SCALE;
	obb.isTrigger   = true;  ///< 当たり判定を貫通させる(押し出さなくなる)

	objPlayerController = GetScene().Instantiate<PlayerControllerNormal>();
	objPlayerHand       = GetScene().Instantiate<PlayerHand>(this->ToGameObject());

	// ジャンプの初期化
	jumpVelocity = 0.0f;
	isJumping    = false;

	// 初期位置の地面の高さを取得
	transform.position.y = Get<TerrainSystem>().GetHeight(transform.position.x, transform.position.z);

	SetJob(PlayerJobType_Healer);
}

void Player::Update()
{
	SessionManager& sessionManager = Get<SessionManager>();
	std::string playerName         = sessionManager.RefStatus().name;

	ImGui::Begin("Player");
	ImGui::DragInt("anim", &animIndex, 1, 0);
	ImGui::DragInt("jobType", &jobType, 1, 0, 3);
	ImGui::DragFloat("animSpeed", &animSpeed, 1.0f, 0.0f);
	ImGui::DragFloat("rotationX", &transform.rotation.x, 1, 0);
	ImGui::DragFloat("rotationY", &transform.rotation.y, 1, 0);
	ImGui::Text("プレイヤー名：%s", playerName.c_str());
	ImGui::End();

	if (Get<InputSystem>().GetKeyDown(KeyCode::J))
	{
		SetJob((jobType + 1) % PlayerJobType_Max);
	}

	nowAttackCoolTime -= Get<DeltaTime>();
	nowDamagedCoolTime -= Get<DeltaTime>();

	// プレイヤーの更新処理
	Get<IPlayerController>(objPlayerController).Update(Data());

	// カメラの更新処理
	Get<IMainCamera>(objMainCamera).UpdatePositionFromTarget(this->ToGameObject());

	if (helthState == HelthState_Live)
	{
		MoveBody();
	}
	else if (helthState == HelthState_Dead)
	{
		DeadAnim();
	}

	/*
	//テスト用
	 // HPゲージ動作確認用：1秒ごとに10ダメージ
	 hpTestTimer += Get<DeltaTime>();

	 if (hpTestTimer >= 1.0f)
	 {
	     Damaged(10);
	     hpTestTimer = 0.0f;
	 }
	*/
}

void Player::Draw()
{
	Get<Dx11Manager>().SetShader(shaderFront);
	Get<Dx11Manager>().SetMaterial(transform.GetMaterial());

	models[jobType].Get().Draw(Get<DeltaTime>() * animSpeed, animIndex);

	const auto& subParts{ models[jobType].Get().subParts };

	for (const auto& parts : subParts)
	{
		Get<Dx11Manager>().SetSurface(parts.mesh);
		Get<Dx11Manager>().SetMaterial(parts.material);
		Get<Dx11Manager>().SetMaterial(parts.transformMaterial);
		Get<Dx11Manager>().Draw();
	}
}

void Player::AttackCollider()
{
	if (nowAttackCoolTime <= 0.0f)
	{
		GetScene().Instantiate<PlayerAttackColl>(transform.position, damage, ATTACK_COLL_LIFE_TIME);

		nowAttackCoolTime = ATTACK_COOL_TIME;
	}
}

void Player::Damaged(
    int32_t damage)
{
	if (nowDamagedCoolTime <= 0.0f && hp > 0)
	{
		hp -= damage;

		if (hp < 0)
		{
			hp = 0;
		}

		nowDamagedCoolTime = DAMAGED_COOL_TIME;
	}

	if (hp <= 0)
	{
		SetHelthState(HelthState_Dead);  // hpが0未満なら死亡
	}
}

void Player::SetHelthState(
    const HelthState _state)
{
	switch (_state)
	{
	case HelthState_Live:
		Get<IMainCamera>(objMainCamera).SetCameraType(MainCameraType_TPS);
		break;
	case HelthState_Dead:
		Get<IMainCamera>(objMainCamera).SetCameraType(MainCameraType_Ghost);
		break;
	default:
		VCU_ASSERT("不正な生存ステータスですよ。");
		return;
	}
	helthState = _state;
}

void Player::SetJob(
    const PlayerJobType _jobType)
{
	// 今のjobこんを消す
	GetScene().Destroy(objJobController);

	jobType = _jobType;
	switch (jobType)
	{
	case PlayerJobType_Healer:
		objJobController = GetScene().Instantiate<PlayerJobControllerHealer>();
		break;
	case PlayerJobType_Tank:
		objJobController = GetScene().Instantiate<PlayerJobControllerTank>();
		break;
	case PlayerJobType_Fighter:
		objJobController = GetScene().Instantiate<PlayerJobControllerFighter>();
		break;
	case PlayerJobType_Archer:
		objJobController = GetScene().Instantiate<PlayerJobControllerArcher>();
		break;
	default:
		VCU_ASSERT(false && "選択不可能な役職です。");
		return;
	}

	Get<IPlayerJobController>(objJobController).Start(Data());
}

void Player::MoveBody()
{
	transform.rotation = Quaternion::FromEuler(Vec3{
	    0.0f,
	    Get<IMainCamera>(objMainCamera).GetAngleY(),
	    0.0f,
	});

	// 入力取得
	AbstructInputSystem& inputSystem = Get<AbstructInputSystem>();

	auto pPlayerInput = inputSystem.GetPlayerInput();

	Vec2 bodyMove = pPlayerInput->GetMoveBody();

	float moveSpeed = jobStatus.moveSpeed;

	// カメラの向きを取得
	float angleY = Get<IMainCamera>(objMainCamera).GetAngleY();

	// カメラの向きから前方向を計算
	Vec3 forward = Vec3{ sinf(angleY), 0.0f, cosf(angleY) };

	// カメラの向きから右方向を計算
	Vec3 right = Vec3{ cosf(angleY), 0.0f, -sinf(angleY) };

	// 入力に応じて移動方向を計算
	Vec3 moveDirection = right * bodyMove.x + forward * bodyMove.y;

	// 地形に沿った移動
	Vec3 nextPosition = transform.position;
	nextPosition += moveDirection * moveSpeed;

	animSpeed = (moveDirection * moveSpeed).Length() / 0.1f;
	if (animSpeed > 0.0f)
	{
		animIndex = PlayerAnimType_Walk;

		// 反対方向への入力があったらアニメーションを反転する
		if (bodyMove.y < 0)
		{
			animSpeed *= -1.0f;
		}
	}
	else
	{
		animIndex = PlayerAnimType_Idle;
		animSpeed = 1.0f;
	}

	// TerrainSystemを取得
	auto& terrain = Get<TerrainSystem>();

	// 許容する傾斜の上限
	// 数値を大きくすると急な坂も登れる
	const float MAX_SLOPE = 0.7f;

	// 移動前の位置
	Vec3 currentPosition = transform.position;

	// ----------------------------------------
	// X方向の移動判定
	// ----------------------------------------
	Vec3 candidateX = transform.position;
	candidateX.x    = nextPosition.x;

	float currentHeightX = terrain.GetHeight(currentPosition.x, currentPosition.z);

	float nextHeightX = terrain.GetHeight(candidateX.x, candidateX.z);

	float moveDistanceX = fabsf(candidateX.x - currentPosition.x);

	float heightDifferenceX = nextHeightX - currentHeightX;

	// 高さの変化が移動距離に対して大きすぎる場合は移動しない
	if (heightDifferenceX <= 0.0f || (moveDistanceX > 0.0001f && heightDifferenceX / moveDistanceX <= MAX_SLOPE))
	{
		transform.position.x = candidateX.x;
	}

	// ----------------------------------------
	// Z方向の移動判定
	// ----------------------------------------
	Vec3 candidateZ = transform.position;
	candidateZ.z    = nextPosition.z;

	float currentHeightZ = terrain.GetHeight(transform.position.x, transform.position.z);

	float nextHeightZ = terrain.GetHeight(candidateZ.x, candidateZ.z);

	float moveDistanceZ = fabsf(candidateZ.z - transform.position.z);

	float heightDifferenceZ = nextHeightZ - currentHeightZ;

	if (heightDifferenceZ <= 0.0f || (moveDistanceZ > 0.0001f && heightDifferenceZ / moveDistanceZ <= MAX_SLOPE))
	{
		transform.position.z = candidateZ.z;
	}

	// 現在位置の地面の高さを取得
	float groundHeight = Get<TerrainSystem>().GetHeight(transform.position.x, transform.position.z);

	// ジャンプしていないときは地面に合わせる
	if (!isJumping)
	{
		if (transform.position.y - groundHeight > 0.1f)
		{
			isJumping = true;
		}
		else if (transform.position.y - groundHeight > 0.0f)
		{
			transform.position.y = groundHeight;
		}
	}

	// ジャンプしていないときは地面の高さに合わせる
	if (!isJumping)
	{
		transform.position.y = groundHeight;
	}

	// ジャンプ処理
	float deltaTime = Get<DeltaTime>();

	// ジャンプボタンを押した瞬間
	if (pPlayerInput->IsJumpBegin() && !isJumping)
	{
		jumpVelocity = JUMP_POWER;
		isJumping    = true;
	}

	// ボタンを離した瞬間
	// 上昇中なら上向きの速度を弱める
	if (pPlayerInput->IsJumpEnd() && jumpVelocity > 0.0f)
	{
		jumpVelocity *= JUMP_CUT_RATE;
	}

	// 上昇・下降処理
	if (isJumping)
	{
		// 重力を適用
		jumpVelocity -= GRAVITY * deltaTime;

		// Y座標を更新
		transform.position.y += jumpVelocity * deltaTime;

		// 地面に着地したら停止

		if (transform.position.y <= groundHeight)
		{
			transform.position.y = groundHeight;
			jumpVelocity         = 0.0f;
			isJumping            = false;
		}

		if (jumpVelocity < 0.0f)
		{
			animIndex = PlayerAnimType_FallBig;
			animSpeed = 1.0f;
		}
		else
		{
			animIndex = PlayerAnimType_Fall;
			animSpeed = 1.0f;
		}
	}
	else
	{
	}

	// お試しで出してるエンティティ
	if (pPlayerInput->IsInteractBegin())
	{
		Get<EntitySystem>().Spawn(
		    1,
		    transform.position + Vec3::Up() * 2.0f,
		    transform.Forward() * 3.0f + Vec3::Up() * 2.0f);
		VCU_LOG("objPawn");
	}

	GameObject objPick = Get<EntitySystem>().PickNearest(transform.position, 3.0f);
	if (objPick)
	{
		// GetScene().Destroy(objPick);
		VCU_LOG("objPick");

		Get<EntitySystem>().Spawn(
		    Get<IEntity>(objPick).GetItemId(),
		    transform.position + Vec3::Up() * 2.0f,
		    transform.Forward() * 3.0f + Vec3::Up() * 2.0f);
		// objPick
	}
}

void Player::DeadAnim()
{
	if (liveStatus.dieAnimTimeSec > 0.0f)
	{
		liveStatus.dieAnimTimeSec -= Get<DeltaTime>();

		animIndex = PlayerAnimType_Die;
		animSpeed = 1.0f;

		if (liveStatus.dieAnimTimeSec <= 0.0f)
		{
			liveStatus.dieAnimTimeSec = 0.0f;

			animIndex = PlayerAnimType_Die;
			animSpeed = 0.0f;

			Get<PlayerHand>(objPlayerHand).SetItem(0);
		}
	}
	else
	{
		animIndex = PlayerAnimType_Die;
		animSpeed = 0.0f;
	}
}

void Player::RegisterStates(
    StateRegistry& registry) const
{
	registry.Register({
	    { "hp", Data().hp },
	    { "helthState", Data().helthState },
	    { "animIndex", Data().animIndex },
	});
}

Vec3 Player::GetPosition() const
{
	return transform.position;
}

float Player::GetAngleY() const
{
	return Vec3{ transform.rotation.ToEuler() }.y;
}