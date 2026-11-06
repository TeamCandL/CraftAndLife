
#pragma once
#include <Game/Game.h>

struct Gauge2Data
{
	Gauge2Data(
	    GameObject _objPlayer,
	    GameObject _objOtherPlayerController,
	    int32_t _playerId) :
	    objPlayer{ _objPlayer },
	    objOtherPlayerController{ _objOtherPlayerController },
	    playerId{ _playerId }
	{
	}

	GameObject objPlayer{};
	GameObject objOtherPlayerController{};

	int32_t playerId{};

	Resource<Texture> texture{ "Image/UI/Gauge2.png" };

	const float maxWidth{ 192.0f };

	Component<UITransform> transform{ Vec2{ 27, 504 }, Vec2{ 192, 24 } };

	Component<UIImage> image{ texture };
};

struct Gauge2 : ObjectBase<Gauge2Data>
{
	using ObjectBase<Gauge2Data>::ObjectBase;

	void Start() override;
	void Update() override;
};
