#include "OtherPlayer.h"

void OtherPlayer::Start()
{
}

void OtherPlayer::Update()
{
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
