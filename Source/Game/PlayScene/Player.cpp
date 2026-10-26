#include "Player.h"
#include "PlayerAttackColl.h"
#include "IMainCamera.h"
#include "Player/PlayerControllerNormal.h"
#include "pch\pch.h"
#include <Game/System/AbstructInputSystem.h>
#include "Entity.h"
#include <Game/PlayScene/TerrainSystem.h>

void Player::Start()
{
	obb.isTrigger = true;

	transform.scale = Vec3::One() * 200.0f;
	obb.offset      = OBB_OFFSET;
	obb.scale       = Vec3::One() * OBB_SCALE;
	obb.isTrigger   = true;  ///< 当たり判定を貫通させる(押し出さなくなる)

	objPlayerController = GetScene().Instantiate<PlayerControllerNormal>();

	// ジャンプの初期化
	jumpVelocity = 0.0f;
	isJumping    = false;

	// 初期位置の地面の高さを取得
	transform.position.y = Get<TerrainSystem>().GetHeight(transform.position.x, transform.position.z);
}

void Player::Update()
{
	ImGui::Begin("Player");
	ImGui::DragInt("anim", &animIndex, 1, 0);
	ImGui::DragFloat("rotationX", &transform.rotation.x, 1, 0);
	ImGui::DragFloat("rotationY", &transform.rotation.y, 1, 0);
	ImGui::End();

	nowAttackCoolTime -= Get<DeltaTime>();
	nowDamagedCoolTime -= Get<DeltaTime>();

	// プレイヤーの更新処理
	Get<IPlayerController>(objPlayerController).Update(Data());

	// カメラの更新処理
	Get<IMainCamera>(objMainCamera).UpdatePositionFromTarget(this->ToGameObject());

	transform.rotation = Quaternion::FromEuler(Vec3{
	    0.0f,
	    Get<IMainCamera>(objMainCamera).GetAngleY(),
	    0.0f,
	});

	// 入力取得
	AbstructInputSystem& inputSystem = Get<AbstructInputSystem>();

	auto pPlayerInput = inputSystem.GetPlayerInput();

	Vec2 bodyMove = pPlayerInput->GetMoveBody();

	float moveSpeed = 0.1f;

	// カメラの向きを取得
	float angleY = Get<IMainCamera>(objMainCamera).GetAngleY();

	// カメラの向きから前方向を計算
	Vec3 forward = Vec3{ sinf(angleY), 0.0f, cosf(angleY) };

	// カメラの向きから右方向を計算
	Vec3 right = Vec3{ cosf(angleY), 0.0f, -sinf(angleY) };

	// 入力に応じて移動方向を計算
	Vec3 moveDirection = right * bodyMove.x + forward * bodyMove.y;

	// 平行移動
	transform.position += moveDirection * moveSpeed;

	// 現在位置の地面の高さを取得
	float groundHeight = Get<TerrainSystem>().GetHeight(transform.position.x, transform.position.z);

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
		float groundHeight = Get<TerrainSystem>().GetHeight(transform.position.x, transform.position.z);

		if (transform.position.y <= groundHeight)
		{
			transform.position.y = groundHeight;
			jumpVelocity         = 0.0f;
			isJumping            = false;
		}
	}

	if (pPlayerInput->IsInteractBegin())
	{
		GetScene().Instantiate<Entity>(1);
	}
}

void Player::Draw()
{
	Get<Dx11Manager>().SetShader(shaderFront);
	Get<Dx11Manager>().SetMaterial(transform.GetMaterial());

	model.Get().Draw(Get<DeltaTime>(), animIndex);

	const auto& subParts{ model.Get().subParts };

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
	if (nowDamagedCoolTime <= 0.0f)
	{
		hp -= damage;
		nowDamagedCoolTime = DAMAGED_COOL_TIME;
	}
}

Vec3 Player::GetPosition() const
{
	return transform.position;
}

float Player::GetAngleY() const
{
	return Vec3{ transform.rotation.ToEuler() }.y;
}