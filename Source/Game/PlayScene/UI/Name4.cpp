#include "pch\pch.h"
#include "Name4.h"
#include <Game/Manager/SessionManager.h>

void Name4::Start() {}

void Name4::Update()
{
	std::vector<std::string> names;
	Get<SessionManager>().ForeachPlayerStatus(
	    [&names](const std::string& name, const InSessionStatus::PlayerStatus& status)
	    {
		    names.push_back(name);
	    });
	if (names.size() <= 0)
	{
		return;
	}
	text.content = names[3];
}
