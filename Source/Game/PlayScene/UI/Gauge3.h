
#pragma once
#include <Game/Game.h>

struct Gauge3Data
{
	Gauge3Data(
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

	Resource<Texture> texture{ "Image/UI/Gauge3.png" };

	const float maxWidth{ 192.0f };

	Component<UITransform> transform{ Vec2{ 94, 571 }, Vec2{ 192, 24 } };

	Component<UIImage> image{ texture };
};

struct Gauge3 : ObjectBase<Gauge3Data>
{
	using ObjectBase<Gauge3Data>::ObjectBase;

	void Start() override;
	void Update() override;
};
