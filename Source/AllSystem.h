#pragma once
#include <cstddef>
#include <type_traits>

struct Canvas;
struct EntitySystem;
struct ItemSystem;
struct TerrainSystem;
struct TutorialSystem;
struct AbstructInputSystem;
struct AnimationSystem;
struct CameraSystem;
struct CursorSystem;
struct ImGuiRenderingSystem;
struct DiscordSystem;
struct EffectSystem;
struct IMESystem;
struct InputSystem;
struct OBBRenderSystem;
struct PadInputSystem;
struct PhysicsSystem;
struct RaycastSystem;
struct ShaderCompiler;
struct ShaderTimeSystem;
struct TranslationSystem;
struct UIRaycasterSystem;
struct UISystem;
struct UITranslationSystem;
struct NullObjectSystem;

namespace cha
{
	template <typename... Ts> struct SystemList
	{
		static constexpr size_t COUNT = sizeof...(Ts);

		template <typename T> static constexpr size_t IndexOf()
		{
			constexpr bool MATCHES[] = { std::is_same_v<T, Ts>... };
			for (size_t i = 0; i < COUNT; ++i)
			{
				if (MATCHES[i])
				{
					return i;
				}
			}
			return COUNT;  // 見つからない
		}

		template <typename T> static constexpr bool Contains()
		{
			return IndexOf<T>() < COUNT;
		}
	};

	// ここにシステムを列挙する。NullObjectSystem は最後
	using AllSystems = SystemList<
	    Canvas,
	    EntitySystem,
	    ItemSystem,
	    TerrainSystem,
	    TutorialSystem,
	    AbstructInputSystem,
	    AnimationSystem,
	    CameraSystem,
	    CursorSystem,
	    ImGuiRenderingSystem,
	    DiscordSystem,
	    EffectSystem,
	    IMESystem,
	    InputSystem,
	    OBBRenderSystem,
	    PadInputSystem,
	    PhysicsSystem,
	    RaycastSystem,
	    ShaderCompiler,
	    ShaderTimeSystem,
	    TranslationSystem,
	    UIRaycasterSystem,
	    UISystem,
	    UITranslationSystem,
	    NullObjectSystem>;
}  // namespace cha