#include "pch\pch.h"
#include "PlayerHand.h"
#include "ItemSystem.h"
#include <Game/System/AbstructInputSystem.h>
#include "Player.h"
#include "ItemAction/IItemAction.h"
#include "ItemAction/ItemActionWeapons.h"
#include "ItemAction/ItemActionWeaponBow.h"

void PlayerHand::Start()
{
	transform.scale = Vec3::One() * 0.005f;

	transformAnimator.animations["idle"] = { TransformAnimationKey{
		0.0f,
		Vec3{ 0.003000f, 0.002000f, 0.001000f },
		Quaternion::FromEuler(Vec3{ 0.681000f, 0.000000f, 0.000000f }),
		Vec3{ 0.005000f, 0.005000f, 0.005000f } } };

	transformAnimator.animations["action"] = {
		TransformAnimationKey{ 0.0f,
		                       Vec3{ 0.003000f, 0.002000f, 0.001000f },
		                       Quaternion::FromEuler(Vec3{ 0.681000f, 0.000000f, 0.000000f }),
		                       Vec3{ 0.005000f, 0.005000f, 0.005000f } },
		TransformAnimationKey{ 0.03f,
		                       Vec3{ 0.003000f, 0.002000f, 0.001000f },
		                       Quaternion::FromEuler(Vec3{ 1.273991f, -1.414000f, 0.000000f }),
		                       Vec3{ 0.005000f, 0.005000f, 0.005000f } },
		/*TransformAnimationKey{ 0.35f,
		                       Vec3{ 0.003000f, 0.002000f, 0.001000f },
		                       Quaternion::FromEuler(Vec3{ -0.959000f, -0.454001f, -0.140001f }),
		                       Vec3{ 0.005000f, 0.005000f, 0.005000f } },*/
		TransformAnimationKey{ 0.4f,
		                       Vec3{ 0.003000f, 0.002000f, 0.001000f },
		                       Quaternion::FromEuler(Vec3{ 0.681000f, 0.000000f, 0.000000f }),
		                       Vec3{ 0.005000f, 0.005000f, 0.005000f } },
	};

	transformAnimator.animations["bowstring"] = {
		TransformAnimationKey{ 0.0f,
		                       Vec3{ 0.002000f, 0.005000f, 0.001000f },
		                       Quaternion::FromEuler(Vec3{ -0.593001f, -0.192000f, 0.348000f }),
		                       Vec3{ 0.005000f, 0.005000f, 0.005000f } },
		TransformAnimationKey{ 0.1f,
		                       Vec3{ 0.002000f, 0.005000f, 0.002000f },
		                       Quaternion::FromEuler(Vec3{ -0.471000f, -0.087000f, 0.453000f }),
		                       Vec3{ 0.005000f, 0.005000f, 0.005000f } },
		TransformAnimationKey{ 0.2f,
		                       Vec3{ 0.002000f, 0.005000f, 0.002000f },
		                       Quaternion::FromEuler(Vec3{ -0.436000f, -0.139000f, 0.261000f }),
		                       Vec3{ 0.005000f, 0.005000f, 0.005000f } },
		TransformAnimationKey{ 0.3f,
		                       Vec3{ 0.002000f, 0.005000f, 0.001000f },
		                       Quaternion::FromEuler(Vec3{ -0.419000f, 0.018000f, 0.261000f }),
		                       Vec3{ 0.005000f, 0.005000f, 0.005000f } },
		TransformAnimationKey{ 0.4f,
		                       Vec3{ 0.002000f, 0.005000f, 0.001000f },
		                       Quaternion::FromEuler(Vec3{ -0.593001f, -0.192000f, 0.348000f }),
		                       Vec3{ 0.005000f, 0.005000f, 0.005000f } },
	};

	transformAnimator.playAnimation = "idle";

	SetItem(3);
}

void PlayerHand::Update(
    PlayerData& _playerData)
{
	ImGui::Begin("PlayerHand");

	ImGui::DragFloat3("offset", transform.position.v, 0.0001f);
	Vec3 angles = transform.rotation.ToEuler();
	ImGui::DragFloat3("angles", angles.v, DirectX::XM_PI / 180.0f);
	transform.rotation = Quaternion::FromEuler(angles);

	if (ImGui::Button("出力"))
	{
		fs::path filePath{ "./animDump.txt" };
		std::ofstream ofs{ filePath, std::ios::app };
		if (ofs)
		{
			ofs << std::format(
			           "TransformAnimationKey{{ 0.0f, Vec3{{ {:f}f, {:f}f, {:f}f }}, Quaternion::FromEuler(Vec3{{ "
			           "{:f}f, {:f}f, "
			           "{:f}f }}), Vec3{{ "
			           "{:f}f, {:f}f, "
			           "{:f}f }} }}",
			           transform.position.x,
			           transform.position.y,
			           transform.position.z,
			           angles.x,
			           angles.y,
			           angles.z,
			           transform.scale.x,
			           transform.scale.y,
			           transform.scale.z)
			    << std::endl;
			/*ofs << std::format(
			           "TransformAnimationKey{ Vec3{{ {}, {}, {} }}, Quaternion::From(Vec3{{ {}, {}, {} }}), Vec3{{ "
			           "{}, {}, "
			           "{} }} }",
			           transform.position.x,
			           transform.position.y,
			           transform.position.z,
			           angles.x,
			           angles.y,
			           angles.z,
			           transform.scale.x,
			           transform.scale.y,
			           transform.scale.z)
			    << std::endl;*/
		}
		ofs.close();
	}

	if (ImGui::Button(isAnimPreviewingDebug ? "アニメーション設定終了" : "アニメーション設定する"))
	{
		isAnimPreviewingDebug = !isAnimPreviewingDebug;

		if (isAnimPreviewingDebug)
		{
			transformAnimator.playAnimation = "";
		}
		else
		{
			transformAnimator.playAnimation = "idle";
		}
	}

	ImGui::End();

	if (isAnimPreviewingDebug)
	{
		transformAnimator.playAnimation = "";
		return;  // アニメーションプレビューデバッグ中なら無視
	}

	if (!objItemAction.IsEmpty())
	{
		Get<IItemAction>(objItemAction).Update(haveItemId, _playerData, Data());
	}
}

void PlayerHand::Draw()
{
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

void PlayerHand::SetItem(
    const ItemId _itemId)
{
	GetScene().Destroy(objItemAction);

	const ItemInfo ITEM_INFO = Get<ItemSystem>().GetInfo(_itemId);
	switch (ITEM_INFO._type)
	{
	case ItemType::Weapons:
	case ItemType::Tools:
		objItemAction = GetScene().Instantiate<ItemActionWeapons>();
		break;
	case ItemType::WeaponBow:
		objItemAction = GetScene().Instantiate<ItemActionWeaponBow>();
		break;
	case ItemType::Consumables:
		break;
	case ItemType::Materials:
		break;
	default:
		break;
	}

	haveItemId = _itemId;
}
