#include "pch\pch.h"
#include "ChatTextLog.h"
#include "ChatTextLogBack.h"
#include <Game/Manager/AudioManager.h>

void ChatTextLog::Start()
{
	objChatTextLogBack = GetScene().Instantiate<ChatTextLogBack>();
}

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

	Get<ChatTextLogBack>(objChatTextLogBack).Data().text.content = text.content;
}

void ChatTextLog::AddLog(
    const std::string& _content,
    bool _playNotice)
{
	logs.emplace_back(SHOW_TIME, _content);

	// 通知音を鳴らす場合のみ再生
	if (_playNotice)
	{
		Get<AudioManager>().Play(noticeSE);
	}
}