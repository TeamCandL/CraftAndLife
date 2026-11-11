#include "DebugPoint.h"

void DebugPoint::Draw()
{
	return;
#ifdef USE_IMGUI
	ImGui::Begin("DebugPoint");
	ImGui::InputFloat3("focusPoint", transform.position.v);
	ImGui::End();
#endif  // _DEBUG

	Get<Dx11Manager>().SetShader(shader);
	Get<Dx11Manager>().SetMaterial(transform.GetMaterial());

	const auto& subParts{ model.Get().subParts };
	for (const auto& parts : subParts)
	{
		Get<Dx11Manager>().SetSurface(parts.mesh);
		Get<Dx11Manager>().SetMaterial(parts.material);
		Get<Dx11Manager>().SetMaterial(parts.transformMaterial);
		Get<Dx11Manager>().Draw();
	}
}

void DebugPoint::OnDestroy() {}
