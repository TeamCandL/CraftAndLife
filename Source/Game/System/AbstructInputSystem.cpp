#include "AbstructInputSystem.h"
#include <Game/System/PadInputSystem.h>

void AbstructInputSystem::Start()
{
}

void AbstructInputSystem::Update()
{
	PadInputSystem& padInputSystem = Get<PadInputSystem>();
	
	if (padInputSystem.IsConnected())
	{
		VCU_LOG("コントローラー接続済み");
		ImGui::Begin("AbstrucInputSystem");
		ImGui::Text("LT %3.2f", padInputSystem.GetLeftTrigger());
		ImGui::Text("RT %3.2f", padInputSystem.GetRightTrigger());

		Vec2 rStick = padInputSystem.GetRightStickXY();
		ImGui::Text("RStick (%3.2f, %3.2f)", rStick.x, rStick.y);

		Vec2 lStick = padInputSystem.GetLeftStickXY();
		ImGui::Text("LStick (%3.2f, %3.2f)", lStick.x, lStick.y);

		ImGui::Text("Button A %s", padInputSystem.IsButton(GameInputGamepadA) ? "ON" : "OFF");
		ImGui::Text("Button B %s", padInputSystem.IsButton(GameInputGamepadB) ? "ON" : "OFF");
		ImGui::Text("Button X %s", padInputSystem.IsButton(GameInputGamepadX) ? "ON" : "OFF");
		ImGui::Text("Button Y %s", padInputSystem.IsButton(GameInputGamepadY) ? "ON" : "OFF");

		ImGui::Text("Button LB %s", padInputSystem.IsButton(GameInputGamepadLeftShoulder) ? "ON" : "OFF");
		ImGui::Text("Button RB %s", padInputSystem.IsButton(GameInputGamepadRightShoulder) ? "ON" : "OFF");
		
		ImGui::End();
	}
	else
	{
		VCU_LOG("コントローラー未接続");
	}
}
