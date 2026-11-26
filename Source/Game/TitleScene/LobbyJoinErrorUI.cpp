#include "LobbyJoinErrorUI.h"

void LobbyJoinErrorUI::Start()
{
	image.texture = texture;
}

void LobbyJoinErrorUI::Update()
{
	ImGuiIO& io = ImGui::GetIO();
	if (target.isPressedThisFrame && !io.WantCaptureMouse)
	{
		Hide();
	}
}

bool LobbyJoinErrorUI::IsShow()
{
	return transform.scale.x > 0.0f;
}

void LobbyJoinErrorUI::Show()
{
	transform.scale = IMAGE_SIZE;
}

void LobbyJoinErrorUI::Hide()
{
	transform.scale = Vec2::Zero();
}
