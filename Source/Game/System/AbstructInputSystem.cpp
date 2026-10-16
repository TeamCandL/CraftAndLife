#include "AbstructInputSystem.h"
#include <Game/System/PadInputSystem.h>
#include "AbstructInputSystem/NormalPlayerInput.h"
#include "AbstructInputSystem/NormalUIInput.h"

void AbstructInputSystem::Start()
{
	SetPlayerInput<NormalPlayerInput>();
	SetUIInput<NormalUIInput>();
}

void AbstructInputSystem::Update()
{
	if (pPlayerInput && pUIInput)
	{
		PadInputSystem& padInputSystem = Get<PadInputSystem>();

		if (padInputSystem.IsConnected())
		{
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

			ImGui::Separator();

			Vec2 bodyMove{ pPlayerInput->GetMoveBody() };
			ImGui::Text("GetMoveBody: (%3.2f, %3.2f)", bodyMove.x, bodyMove.y);
			Vec2 cameraMove{ pPlayerInput->GetMoveCamera() };
			ImGui::Text("GetMoveCamera: (%3.2f, %3.2f)", cameraMove.x, cameraMove.y);
			ImGui::Text("IsActionTrigger: %3.2f", pPlayerInput->GetActionTrigger());
			ImGui::Text("IsActionBegin: %s", pPlayerInput->IsActionBegin() ? "TRUE" : "FALSE");
			ImGui::Text("IsAction: %s", pPlayerInput->IsAction() ? "TRUE" : "FALSE");
			ImGui::Text("IsActionEnd: %s", pPlayerInput->IsActionEnd() ? "TRUE" : "FALSE");
			ImGui::Text("IsSubActionTrigger: %3.2f", pPlayerInput->GetSubActionTrigger());
			ImGui::Text("IsSubActionBegin: %s", pPlayerInput->IsSubActionBegin() ? "TRUE" : "FALSE");
			ImGui::Text("IsSubAction: %s", pPlayerInput->IsSubAction() ? "TRUE" : "FALSE");
			ImGui::Text("IsSubActionEnd: %s", pPlayerInput->IsSubActionEnd() ? "TRUE" : "FALSE");
			ImGui::Text("IsMovedHotbarLeft: %s", pPlayerInput->IsMovedHotbarLeft() ? "TRUE" : "FALSE");
			ImGui::Text("IsMovedHotbarRight: %s", pPlayerInput->IsMovedHotbarRight() ? "TRUE" : "FALSE");
			ImGui::Text("IsInteractBegin: %s", pPlayerInput->IsInteractBegin() ? "TRUE" : "FALSE");
			ImGui::Text("IsInteract: %s", pPlayerInput->IsInteract() ? "TRUE" : "FALSE");
			ImGui::Text("IsInteractEnd: %s", pPlayerInput->IsInteractEnd() ? "TRUE" : "FALSE");
			ImGui::Text("IsJumpBegin: %s", pPlayerInput->IsJumpBegin() ? "TRUE" : "FALSE");
			ImGui::Text("IsJump: %s", pPlayerInput->IsJump() ? "TRUE" : "FALSE");
			ImGui::Text("IsJumpEnd: %s", pPlayerInput->IsJumpEnd() ? "TRUE" : "FALSE");

			ImGui::Separator();

			ImGui::Text("IsMovedUp: %s", pUIInput->IsMovedUp() ? "TRUE" : "FALSE");
			ImGui::Text("IsMovedDown: %s", pUIInput->IsMovedDown() ? "TRUE" : "FALSE");
			ImGui::Text("IsMovedLeft: %s", pUIInput->IsMovedLeft() ? "TRUE" : "FALSE");
			ImGui::Text("IsMovedRight: %s", pUIInput->IsMovedRight() ? "TRUE" : "FALSE");
			ImGui::Text("IsSelected: %s", pUIInput->IsSelected() ? "TRUE" : "FALSE");
			ImGui::Text("IsChanneled: %s", pUIInput->IsChanneled() ? "TRUE" : "FALSE");

			ImGui::End();
		}
		else
		{
			VCU_LOG("コントローラー未接続");
		}
	}
}

PadInputSystem& AbstructInputSystem::GetPadInputSystem()
{
	return Get<PadInputSystem>();
}

RawMouseInputManager& AbstructInputSystem::GetRawMouseInputManager()
{
	return Get<RawMouseInputManager>();
}

InputSystem& AbstructInputSystem::GetInputSystem()
{
	return Get<InputSystem>();
}
