#pragma once
#include <Game/Game.h>
#include <array>

struct LobbyUIData
{
	LobbyUIData(
	    GameObject _objRoleSelectButton) :
	    objRoleSelectButton{ _objRoleSelectButton }
	{
	}

	GameObject objRoleSelectButton{};

	GameObject lobbyLabelText{};
	GameObject teamNameLabelText{};
	GameObject teamNameText{};
	GameObject playerCountText{};

	struct PlayerParameter
	{
		GameObject playerNameText{};
		GameObject readyText{};
		GameObject jobLabelText{};
		GameObject jobNameText{};
	};
	static constexpr int PLAYER_PARAMETER_FONT_SIZE{ 32 };
	Vec2 playerNameBegin{};
	Vec2 readyTextBegin{};
	Vec2 jobLabelBegin{};
	Vec2 jobNameBegin{};

	static constexpr int PLAYER_COUNT{ 4 };
	std::array<PlayerParameter, PLAYER_COUNT> playerLists;
	int playerParamOffsetY{ 93 };

	Resource<Texture> backgroundTexutre{ "Image/Lobby/LobbyBackground.png" };
	Component<UIImage> backgroundImage{ backgroundTexutre };
	Component<UITransform> backgroundTransform{};
};

struct LobbyUI : ObjectBase<LobbyUIData>
{
	using ObjectBase<LobbyUIData>::ObjectBase;
	void Start();
	void Update();

	void SetTextObjectFromJson(const nlohmann::json& _json, const GameObject& _obj);

	void SetImageObjectFromJson(const nlohmann::json& _json, const GameObject& _obj);
};