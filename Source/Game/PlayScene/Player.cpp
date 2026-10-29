#include "Player.h"
#include "IMainCamera.h"
#include "Player/PlayerControllerNormal.h"
#include "pch\pch.h"
#include <Game/System/AbstructInputSystem.h>

void Player::Start()
{
	transform.scale     = Vec3::One() * 200.0f;
	objPlayerController = GetScene().Instantiate<PlayerControllerNormal>();
	laserEffect         = GetScene().Instantiate<EffectObject>("Effect/Hit.efkefc");
}

void Player::Update()
{
	ImGui::Begin("Player");
	ImGui::DragInt("anim", &animIndex, 1, 0);
	ImGui::DragFloat("rotationX", &transform.rotation.x, 1, 0);
	ImGui::DragFloat("rotationY", &transform.rotation.y, 1, 0);
	ImGui::End();

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
	auto pPlayerInput                = inputSystem.GetPlayerInput();
	Vec2 bodyMove                    = pPlayerInput->GetMoveBody();

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

	auto& data = Get<EffectObject>(laserEffect).Data();

	if (Get<InputSystem>().GetKeyDown(KeyCode::O))
	{

		Get<EffectObject>(laserEffect).Play();
		Get<EffectObject>(laserEffect).Data().effectPlayer.isLoop = true;
		Get<EffectObject>(laserEffect).Data().transform.position  = transform.position;
		Get<EffectObject>(laserEffect).Data().transform.scale     = Vec3::One() * 1.0f;
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
	// new PlayerAttackColl();
}

void Player::Damaged(
    int32_t damage)
{
	hp -= damage;
}

Vec3 Player::GetPosition() const
{
	return transform.position;
}

float Player::GetAngleY() const
{
	return Vec3{ transform.rotation.ToEuler() }.y;
}
