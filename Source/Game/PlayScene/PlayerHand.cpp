#include "pch\pch.h"
#include "PlayerHand.h"
#include "ItemSystem.h"

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
		TransformAnimationKey{ 0.04f,
		                       Vec3{ 0.003000f, 0.002000f, 0.001000f },
		                       Quaternion::FromEuler(Vec3{ 1.273991f, -1.414000f, 0.000000f }),
		                       Vec3{ 0.005000f, 0.005000f, 0.005000f } },
		TransformAnimationKey{ 0.1f,
		                       Vec3{ 0.003000f, 0.002000f, 0.001000f },
		                       Quaternion::FromEuler(Vec3{ -0.959000f, -0.454001f, -0.140001f }),
		                       Vec3{ 0.005000f, 0.005000f, 0.005000f } },
		TransformAnimationKey{ 0.2f,
		                       Vec3{ 0.003000f, 0.002000f, 0.001000f },
		                       Quaternion::FromEuler(Vec3{ 0.681000f, 0.000000f, 0.000000f }),
		                       Vec3{ 0.005000f, 0.005000f, 0.005000f } },
	};

	transformAnimator.playAnimation = "idle";
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

	ImGui::End();

	if (isActing)
	{
		// Get<ItemSystem>();
		if (transformAnimator.isFinished)
		{
			VCU_LOG("アクション！");
		}
	}
	else
	{
		if (transformAnimator.isFinished)
		{
			transformAnimator.playAnimation  = "idle";
			transformAnimator.playingTimeSec = 0.0f;
		}
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

void PlayerHand::SetAction(
    const bool _isAction)
{
	if (_isAction)
	{
		if (!isActing)
		{
			isActing                         = true;
			transformAnimator.playingTimeSec = 0.0f;
			transformAnimator.playAnimation  = "action";
		}

		transformAnimator.playToLoop = true;  // 長押し中ならループ追加を要請
	}
	else
	{
		if (isActing)
		{
			isActing = false;
			// transformAnimator.playingTimeSec = 0.0f;
			// transformAnimator.playAnimation  = "idle";
			transformAnimator.playToLoop = false;
		}
	}
}

void PlayerHand::SetItem(
    const ItemId _itemId)
{
	haveItemId = _itemId;
}
