#include "pch\pch.h"
#include "OtherPlayerHand.h"
#include "ItemSystem.h"
#include <Game/Manager/SessionManager.h>

void OtherPlayerHand::Start()
{
	selfName = Get<Name>(transform.parent).content;
}

void OtherPlayerHand::Update()
{
	const InSessionStatus::PlayerStatus& status = Get<SessionManager>().RefPlayerStatus(selfName);

	transform.rotation = Quaternion::FromEuler(Vec3{ status.rotation.x, 0.0f, 0.0f });
	haveItemId         = status.haveItemId;
}

void OtherPlayerHand::Draw()
{
	if (!haveItemId)
	{
		return;  // アイテムを持っていないなら無視
	}

	Resource<Model> hModel = Get<ItemSystem>().GetModel(haveItemId);

	if (hModel.IsNull())
	{
		return;
	}

	Get<Dx11Manager>().SetShader(shader);
	Get<Dx11Manager>().SetMaterial(transform.GetMaterial());

	// hModel.Get().Draw(Get<DeltaTime>() * animSpeed, animIndex);

	const auto& subParts{ hModel.Get().subParts };

	for (const auto& parts : subParts)
	{
		Get<Dx11Manager>().SetSurface(parts.mesh);
		Get<Dx11Manager>().SetMaterial(parts.material);
		Get<Dx11Manager>().SetMaterial(parts.transformMaterial);
		Get<Dx11Manager>().Draw();
	}
}
