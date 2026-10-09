#include "WordInputPanel.h"

void WordInputPanel::Start()
{
	image.texture = texture;
}

void WordInputPanel::Update()
{
	ImGuiIO& io = ImGui::GetIO();
	if (target.isPressedThisFrame && !io.WantCaptureMouse)
	{
		Hide();
	}
}

bool WordInputPanel::IsShown()
{
	return transform.scale.x > 0.0f;
}

void WordInputPanel::Show()
{
	transform.scale = IMAGE_SIZE;
}

void WordInputPanel::Hide()
{
	transform.scale = Vec2::Zero();
}
