#include "LobbyJoinErrorUI.h"
#include <Game/Manager/SessionManager.h>

void LobbyJoinErrorUI::Start()
{
	image.texture = texture;
	Hide();
}

void LobbyJoinErrorUI::Update()
{
	SessionManager& session       = Get<SessionManager>();
	const InSessionStatus& status = session.RefStatus();

	if (!status.joinErrorTypes.empty())
	{
		// エラー内容に応じてメッセージを設定
		Show();

		// 表示済みなのでクリア
		session.ResetJoinError();
	}

	ImGuiIO& io = ImGui::GetIO();
	if (target.isPressedThisFrame && !io.WantCaptureMouse)
	{
		Hide();
	}
}

bool LobbyJoinErrorUI::IsShow()
{
	return transform.scale.x > 0.0f;
}

void LobbyJoinErrorUI::Show()
{
	transform.scale = IMAGE_SIZE;
}

void LobbyJoinErrorUI::Hide()
{
	transform.scale = Vec2::Zero();
}

void LobbyJoinErrorUI::SetErrorText(
    const std::vector<int32_t>& errors)
{
	std::string message;

	for (const int32_t error : errors)
	{
		switch (static_cast<JoinError>(error))
		{
		case JoinError::NameDuplicate:
			message += "そのプレイヤー名は既に使用されています\n";
			break;

		case JoinError::LobbyFull:
			message += "ロビーが満員です\n";
			break;

		default:
			break;
		}
	}

	// ここでテキストUIにメッセージを設定する
}
