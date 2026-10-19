#include "OtherPlayer.h"
#include <Game/Manager/SessionManager.h>

void OtherPlayer::Start()
{
}

void OtherPlayer::Update()
{
	SessionManager& sessionManager = Get<SessionManager>();
	if (sessionManager.IsExistPlayer(name.content)) // プレイヤーが存在するなら
	{
		const InSessionStatus::PlayerStatus& status = sessionManager.RefPlayerStatus(name.content);

		// 座標と角度を更新
		transform.position = status.position;
		transform.rotation = Quaternion::FromEuler(Vec3{0.0f, status.rotation.y, 0.0f});
	}
}

void OtherPlayer::Draw()
{
	Get<Dx11Manager>().SetShader(shader);
	Get<Dx11Manager>().SetMaterial(transform.GetMaterial());

	models[role].Get().Draw(Get<DeltaTime>(), animIndex);

	const auto& subParts{models[role].Get().subParts};
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
	return Vec3{transform.rotation.ToEuler()}.y;
}
