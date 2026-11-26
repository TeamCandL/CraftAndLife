
#pragma once
#include <Game/Game.h>

struct Gauge4Data
{
	Gauge4Data(
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

	Resource<Texture> texture{ "Image/UI/Gauge4.png" };

	const float maxWidth{ 192.0f };

	Component<UITransform> transform{ Vec2{ 94, 648 }, Vec2{ 192, 24 } };

	Component<UIImage> image{ texture };
};

struct Gauge4 : ObjectBase<Gauge4Data>
{
	using ObjectBase<Gauge4Data>::ObjectBase;

	void Start() override;
	void Update() override;
};
