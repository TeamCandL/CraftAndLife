#include "pch\pch.h"
#include "PlayerHand.h"
#include "ItemSystem.h"

void PlayerHand::Start()
{
	transform.scale = Vec3::One() * 0.005f;
}

void PlayerHand::Update()
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
			           "TransformAnimationKey{{ 0.0f, Vec3{{ {:3.2f}f, {}f, {}f }}, Quaternion::From(Vec3{{ {}f, {}f, "
			           "{}f }}), Vec3{{ "
			           "{}f, {}f, "
			           "{}f }} }}",
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

	ImGui::End();
}

void PlayerHand::Draw()
{
	Resource<Model> hModel = Get<ItemSystem>().GetModel(haveItemId);

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
	haveItemId = _itemId;
}
