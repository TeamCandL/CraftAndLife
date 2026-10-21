#include "Player.h"
#include "PlayerAttackColl.h"
#include "IMainCamera.h"
#include "Player/PlayerControllerNormal.h"
#include "pch\pch.h"
#include <Game/System/AbstructInputSystem.h>

void Player::Start()
{
	transform.scale = Vec3::One() * 200.0f;
	objPlayerController = GetScene().Instantiate<PlayerControllerNormal>();
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

	AbstructInputSystem& inputSystem = Get<AbstructInputSystem>();
	auto pPlayerInput = inputSystem.GetPlayerInput();
	Vec2 bodyMove = pPlayerInput->GetMoveBody();

	float moveSpeed = 0.1f;

	// カメラの向きを取得
	float angleY = Get<IMainCamera>(objMainCamera).GetAngleY();

	// カメラの向きから前方向を計算
	Vec3 forward = Vec3{sinf(angleY), 0.0f, cosf(angleY)};

	// カメラの向きから右方向を計算
	Vec3 right = Vec3{cosf(angleY), 0.0f, -sinf(angleY)};

	// 入力に応じて移動方向を計算
	Vec3 moveDirection = right * bodyMove.x + forward * bodyMove.y;

	// 平行移動
	transform.position += moveDirection * moveSpeed;

	//ジャンプ処理
	pPlayerInput->IsJumpBegin();
	pPlayerInput->IsJump();
	pPlayerInput->IsJumpEnd();
}

void Player::Draw()
{
	Get<Dx11Manager>().SetShader(shaderFront);
	Get<Dx11Manager>().SetMaterial(transform.GetMaterial());

	model.Get().Draw(Get<DeltaTime>(), animIndex);

	const auto& subParts{model.Get().subParts};
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

void Player::Damaged(int32_t damage)
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
	return Vec3{transform.rotation.ToEuler()}.y;
}
