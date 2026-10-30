#include "pch\pch.h"
#include "Name2.h"
#include <Game/Manager/SessionManager.h>

void Name2::Start() {}

void Name2::Update()
{
	std::vector<std::string> names;
	Get<SessionManager>().ForeachPlayerStatus(
	    [&names](const std::string& name, const InSessionStatus::PlayerStatus& status)
	    {
		    names.push_back(name);
	    });
	if (names.size() <= 1)
	{
		return;
	}
	text.content = names[1];
}
