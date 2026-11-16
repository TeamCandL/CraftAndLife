#pragma once
#include <Game/Game.h>

struct ClearSceneInputData
{
	float elapsedTime{ 0.0f };
	float inputDelay{ 4.0f };
	GameObject objInputBackground{};
	GameObject objInputText{};
	Resource<ResourceType::Texture> backgroundTexture{ "Image/Lobby/LobbyBackground.png" };
};

struct ClearSceneInput : ObjectBase<ClearSceneInputData>
{
	void Start() override;
	void Update() override;
};