#include "LobbyReadyButton.h"
#include <Game/Common/UI/Button.h>
#include <Game/Network/NetworkSessionManager.h>
#include <Game/Network/Protocols/ReadySelf.h>
#include <Game/Network/Protocols/StartGame.h>
#include <Game/Manager/SessionManager.h>
#include <unordered_set>

void LobbyReadyButton::Start()
{

	FontLoadConfig config{};
	config.fontSize = 64;
	config.WithFontAlignment(cha::FontAlignment_::FontAlignment_Center);
	Resource<ResourceType::Font> font{ config };
	objButton = GetScene().Instantiate<UI::Button>(
	    textureNormalFileName,
	    textureSelectedFileName,
	    textureSelectedFileName,
	    "",
	    begin,
	    scale,
	    font);
}

void LobbyReadyButton::Update()
{
	using enum NetworkSessionManager::State;
	auto& status = Get<SessionManager>().RefStatus();
	if (status.imLobbyHost)
	{
		if (showDuplicateRoleMessage)
		{
			Get<UIText>(objButton).content = "役職が重複しています！";
		}
		else if (isStatable)
		{
			Get<UIText>(objButton).content = "ゲーム開始";
		}
		else
		{
			Get<UIText>(objButton).content = "準備完了とする";
		}
	}
	else
	{
		Get<UIText>(objButton).content = "待機中...";
	}
	if (Get<UI::Button>(objButton).IsPushed() || Get<InputSystem>().GetKeyDown(KeyCode::S))
	{
		NetworkSessionManager& networkSessionManager = Get<NetworkSessionManager>();

		if (networkSessionManager.GetCurrentState() == InLobbyNotReady)
		{
			isStatable = true;
			networkSessionManager.SetCurrentState(InLobbyWaiting);

			Protocol::ReadySelf message{};
			networkSessionManager.Send(message);
		}
		else
		{
			// 役職の重複チェック
			std::unordered_set<int32_t> selectedRoles;

			bool hasDuplicateRole = false;

			for (const auto& [playerId, roleId] : status.memberRoles)
			{
				if (!selectedRoles.insert(roleId).second)
				{
					hasDuplicateRole = true;

					VCU_LOG("役職重複: PlayerID={}, RoleID={}", playerId, roleId);
					break;
				}
			}

			if (hasDuplicateRole)
			{
				showDuplicateRoleMessage = true;

				VCU_LOG("役職が重複しています。ゲームを開始できません。");
				// メッセージボックス出したいねー
				return;
			}

			// 重複がなければゲーム開始
			showDuplicateRoleMessage = false;

			Protocol::StartGame message{};
			networkSessionManager.Send(message);
		}

		networkSessionManager.Flush();
	}
}
