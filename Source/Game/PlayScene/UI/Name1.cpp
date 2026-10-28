#include "pch\pch.h"
#include "Name1.h"
#include <Game/Manager/SessionManager.h>

void Name1::Start() {}

void Name1::Update()
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
	text.content = "aaaa";
}
