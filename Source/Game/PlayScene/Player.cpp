#include "pch\pch.h"
#include "Player.h"
#include "Player/PlayerControllerNormal.h"

void Player::Start()
{
	transform.scale = Vec3::One() * 200.0f;
	objPlayerController = GetScene().Instantiate<PlayerControllerNormal>();
}

void Player::Update()
{
	ImGui::Begin("Player");
	ImGui::DragInt("anim", &animIndex, 1, 0);
	ImGui::End();

	// プレイヤーの更新処理
	Get<IPlayerController>(objPlayerController).Update(Data());
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
