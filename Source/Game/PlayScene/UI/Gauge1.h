#pragma once
#include <Game/Game.h>

struct Gauge1Data
{
	Gauge1Data(
	    GameObject _objPlayer,
	    GameObject _objOtherPlayerController,
	    int32_t _playerId) :
	    objPlayer{ _objPlayer },
	    objOtherPlayerController{ _objOtherPlayerController },
	    playerId{ _playerId }
	{
	}

	// 自分のプレイヤー
	GameObject objPlayer{};

	// 他プレイヤー管理オブジェクト
	GameObject objOtherPlayerController{};

	// このゲージが担当するプレイヤーID
	int32_t playerId{};

	Resource<Texture> texture{ "Image/UI/Gauge1.png" };

	const float maxWidth{ 192.0f };

	Component<UITransform> transform{ Vec2{ 94, 417 }, Vec2{ 192, 24 } };

	Component<UIImage> image{ texture };
};

struct Gauge1 : ObjectBase<Gauge1Data>
{
	using ObjectBase<Gauge1Data>::ObjectBase;

	void Start() override;
	void Update() override;
};