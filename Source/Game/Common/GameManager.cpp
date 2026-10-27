
#include "GameManager.h"
#include "../System/InputSystem.h"
#include "../System/AudioSystem.h"
#include "../System/DiscordSystem.h"
#include "../System/TranslationSystem.h"
#include "../System/CameraSystem.h"
#include "../System/CursorSystem.h"
#include "../System/PhysicsSystem.h"
#include "../System/OBBRenderSystem.h"
#include "../System/UISystem.h"
#include "../System/RaycastSystem.h"
#include "../System/ShaderTimeSystem.h"
#include "../System/UITranslationSystem.h"

#include <Game/Common/Canvas.h>

#include "../System/DebugSystems/ImGuiRenderingSystem.h"

#include <Base/Manager/IssueManager.h>
#include <Base/Manager/SceneManager.h>
#include <Game/System/IMESystem.h>
#include <Game/System/PadInputSystem.h>
#include <Game/System/AbstructInputSystem.h>
#include <Game/PlayScene/TerrainSystem.h>
#include <Game/PlayScene/ItemSystem.h>
#include <Game/PlayScene/EntitySystem.h>

void GameManager::Start()
{
	GetScene().Instantiate<InputSystem>();
	GetScene().Instantiate<PadInputSystem>();
	GetScene().Instantiate<AbstructInputSystem>();
	GetScene().Instantiate<CursorSystem>();
	GetScene().Instantiate<AudioSystem>();
	// GetScene().Instantiate<DiscordSystem>();
	GetScene().Instantiate<TranslationSystem>();
	GetScene().Instantiate<PhysicsSystem>();
	GetScene().Instantiate<RaycastSystem>();

	GetScene().Instantiate<CameraSystem>();
	GetScene().Instantiate<ShaderTimeSystem>();
	GetScene().Instantiate<OBBRenderSystem>();

	GetScene().Instantiate<UITranslationSystem>();

	GetScene().Instantiate<UISystem>();

	GetScene().Instantiate<Canvas>();

	GetScene().Instantiate<ImGuiRenderingSystem>();

	GetScene().Instantiate<IMESystem>();

	GetScene().Instantiate<TerrainSystem>();
	GetScene().Instantiate<ItemSystem>();
	GetScene().Instantiate<EntitySystem>();
}

void GameManager::Update()
{
	InputSystem& input = Get<InputSystem>();

	// Escapeキーが押されたらゲームを終了する
	if (input.GetKey(KeyCode::Escape) && input.GetKey(KeyCode::LeftShift))
	{
		Get<IssueManager>().Error(L"ゲーム終了");
	}
}
