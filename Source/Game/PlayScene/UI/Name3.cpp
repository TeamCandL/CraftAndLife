#include "pch\pch.h"
#include "Name3.h"
#include <Game/Manager/SessionManager.h>

void Name3::Start()
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
	text.content = names[2];
}

void Name3::Update() {}
