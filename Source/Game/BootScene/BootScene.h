#pragma once
#include <Game/Game.h>

/// @brief 起動時のシーン
struct BootScene : SceneBase
{
	/// @brief 開始処理
	void Start() override;

	// 起動音
	Resource<ResourceType::Audio> startupSE{ "BGM_SE/Startup.mp3" };

	// エンジン起動音
	Resource<ResourceType::Audio> chaEngineSE{ "BGM_SE/ChaEngine.mp3" };
};