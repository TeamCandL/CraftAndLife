#include "IMESystem.h"
#include <Base/Manager/WinMessage.h>

void IMESystem::Start()
{
}

void IMESystem::Update()
{
	// TODO: とくに文字入力使わない！
	/*if (Get<InputSystem>().GetKeyDown(KeyCode::Enter))
	{
		Get<WinMessage>().BeginIME();
	}*/

	//Get<WinMessage>().
	//OutputDebugString(std::format(L"{}\r\n", Get<WinMessage>().GetIME()).c_str());
}
