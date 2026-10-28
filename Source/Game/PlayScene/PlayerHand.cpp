#include "pch\pch.h"
#include "PlayerHand.h"

void PlayerHand::Start() {}

void PlayerHand::Update()
{
	ImGui::Begin("PlayerHand");

	ImGui::DragFloat3("offset", transform.position.v, 0.01f);

	ImGui::End();
}

void PlayerHand::Draw() {}
