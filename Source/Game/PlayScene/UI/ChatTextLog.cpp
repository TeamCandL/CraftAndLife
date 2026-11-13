#include "pch\pch.h"
#include "ChatTextLog.h"

void ChatTextLog::Start() {}

void ChatTextLog::Update()
{
	const float DELTA_TIME{ Get<DeltaTime>() };
	for (auto& [timeLeft, content] : logs)
	{
		timeLeft -= DELTA_TIME;
	}

	std::erase_if(
	    logs,
	    [DELTA_TIME](const std::pair<float, std::string>& _log) -> bool
	    {
		    return _log.first <= 0.0f;
	    });

	text.content = "";
	for (auto& [timeLeft, content] : logs)
	{
		text.content += std::format("{} \n", content);
	}
}

void ChatTextLog::AddLog(
    const std::string& _content)
{
	logs.emplace_back(SHOW_TIME, _content);
}
