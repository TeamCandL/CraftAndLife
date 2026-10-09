#include "pch\pch.h"
#include "Player.h"
#include "Player/PlayerControllerNormal.h"

void Player::Start()
{
	transform.scale = Vec3::One() * 0.01f;
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
	Get<Dx11Manager>().EnqueueTransparent([this]
	{
		// ブレンドステートは透明度考慮するやつ
		Get<Dx11Manager>().SetBlendState(BlendStateType_Alpha);
		// 深度バッファは書き込まない
		Get<Dx11Manager>().ChangeDepthState(DepthStateType_Background);

		// Transformをコンスタントバッファに送る
		Get<Dx11Manager>().SetMaterial(transform.GetMaterial());

		model.Get().UpdateAnim(Get<DeltaTime>(), 0);

		// 各モデルのパーツを取ってくる
		const auto& subParts{ model.Get().subParts };
		// 各モデルのパーツを描画しまくる
		auto DrawParts = [this, subParts]
		{
			for (const auto& parts : subParts)
			{
				Get<Dx11Manager>().SetSurface(parts.mesh);
				Get<Dx11Manager>().SetMaterial(parts.material);
				Get<Dx11Manager>().SetMaterial(parts.transformMaterial);
				Get<Dx11Manager>().Draw();
			}
		};

		// 表描画
		Get<Dx11Manager>().SetShader(shaderFront);
		DrawParts();

		Get<Dx11Manager>().SetShader(shaderBack);
		Get<Dx11Manager>().ChangeDepthState(DepthStateType_Standard);
		DrawParts();

		// 深度バッファもブレンドモードも元に戻す
		Get<Dx11Manager>().ChangeDepthState(DepthStateType_Standard);
		Get<Dx11Manager>().SetBlendState(BlendStateType_None);
	});
}
