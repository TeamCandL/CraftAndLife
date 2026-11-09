#include "pch\pch.h"
#include "LobbyUI.h"
#include <Game/UI/ImageObject.h>
#include <Game/UI/TextObject.h>
#include <fstream>
#include <Game/Manager/SessionManager.h>
#include <Game/PlayScene/Player/PlayerJobType.h>

std::string GetJobName(
    PlayerJobType _jobType)
{
	switch (_jobType)
	{
	case PlayerJobType_Healer:  ///< 回復使い
		return "ヒーラー";
		break;
	case PlayerJobType_Tank:  ///< 盾使い
		return "タンク";
		break;
	case PlayerJobType_Fighter:  ///< 大剣使い
		return "ファイター";
		break;
	case PlayerJobType_Archer:  ///< 弓使い
		return "アーチャー";
		break;
	case PlayerJobType_Max:  ///< 最大数
	default:
		return "無効";
		break;
	}
}

void LobbyUI::Start()
{
	backgroundTransform.position = Vec2{ 0, 0 };
	backgroundTransform.scale    = Vec2{ 1920, 1080 };

	std::ifstream i("UI/LobbyUI.json");
	nlohmann::json j;
	i >> j;

	lobbyLabelText = GetScene().Instantiate<TextObject>();
	SetTextObjectFromJson(j["lobby_label"], lobbyLabelText);

	teamNameLabelText = GetScene().Instantiate<TextObject>();
	SetTextObjectFromJson(j["team_name_label"], teamNameLabelText);

	teamNameText = GetScene().Instantiate<TextObject>();
	SetTextObjectFromJson(j["team_name"], teamNameText);

	playerCountText = GetScene().Instantiate<TextObject>();
	SetTextObjectFromJson(j["player_count_label"], playerCountText);

	// プレイヤーのパラメーター(名前や参加状況、ジョブ名)

	/// -------------------
	/// 一つ目の要素の位置
	/// -------------------
	{
		nlohmann::json elementJ = j["player_name_first_element"];
		playerNameBegin.x       = elementJ.at("x").get<int>();
		playerNameBegin.y       = elementJ.at("y").get<int>();
	}
	{
		nlohmann::json elementJ = j["player_ready_first_element"];
		readyTextBegin.x        = elementJ.at("x").get<int>();
		readyTextBegin.y        = elementJ.at("y").get<int>();
	}
	{
		nlohmann::json elementJ = j["player_job_label_first_element"];
		jobLabelBegin.x         = elementJ.at("x").get<int>();
		jobLabelBegin.y         = elementJ.at("y").get<int>();
	}
	{
		nlohmann::json elementJ = j["player_job_first_element"];
		jobNameBegin.x          = elementJ.at("x").get<int>();
		jobNameBegin.y          = elementJ.at("y").get<int>();
	}
	FontLoadConfig config;
	config.fontSize = PLAYER_PARAMETER_FONT_SIZE;

	Resource<ResourceType::Font> font{ config };

	// それぞれの要素の位置を設定
	for (int i = 0; i < playerLists.size(); i++)
	{
		Vec2 offset{ 0, playerParamOffsetY + PLAYER_PARAMETER_FONT_SIZE };
		Vec2 size{ PLAYER_PARAMETER_FONT_SIZE, PLAYER_PARAMETER_FONT_SIZE };
		PlayerParameter param{
			.playerNameText = GetScene().Instantiate<TextObject>(playerNameBegin + offset * i, size, 3, font),
			.readyText      = GetScene().Instantiate<TextObject>(readyTextBegin + offset * i, size, 3, font),
			.jobLabelText   = GetScene().Instantiate<TextObject>(jobLabelBegin + offset * i, size, 3, font),
			.jobNameText    = GetScene().Instantiate<TextObject>(jobNameBegin + offset * i, size, 3, font),
		};

		playerLists[i] = param;
	}
}

void LobbyUI::Update()
{
	auto status                                 = Get<SessionManager>().RefStatus();
	const std::vector<std::string>& memberNames = status.memberNames;
	constexpr float fontSize{ PLAYER_PARAMETER_FONT_SIZE };
	for (int i = 0; i < memberNames.size(); i++)
	{
		// プレイヤー名
		Get<UIText>(playerLists[i].playerNameText).content = memberNames[i];

		auto width                                            = fontSize * memberNames[i].size();
		Get<UITransform>(playerLists[i].playerNameText).scale = Vec2{ static_cast<float>(width), fontSize };

		// 準備状態
		Get<TextObject>(playerLists[i].readyText).SetContent("準備中?");

		Get<TextObject>(playerLists[i].jobLabelText).SetContent("ジョブ");

		// ジョブ名
		Get<TextObject>(playerLists[i].jobNameText).SetContent(GetJobName(status.memberId[i]));
	}
	// ルーム名
	Get<TextObject>(teamNameText).SetContent(status.joinRoomName);

	Get<TextObject>(playerCountText).SetContent(std::format("プレイヤー一覧(最大{}人)", PLAYER_COUNT));
}

void LobbyUI::SetTextObjectFromJson(
    const nlohmann::json& _json,
    const GameObject& _obj)
{
	from_json(_json, Get<TextObject>(_obj).Data());
}

void LobbyUI::SetImageObjectFromJson(
    const nlohmann::json& _json,
    const GameObject& _obj)
{
	from_json(_json, Get<ImageObject>(_obj).Data());
}
