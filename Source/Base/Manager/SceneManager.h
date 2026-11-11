#pragma once
#include "../Core/IScene.h"
#include "IGameLoopCallable.h"
#include "../Core/GameObject.h"
#include "../Core/SystemRegistry.h"
#include "IManager.h"
#include "../Core/ComponentPool/View.h"
#include <tuple>
#include <utility>
#include <functional>
#include <memory>

namespace cha
{
	class IScene;

	class SceneManager : public IGameLoopCallable, public IManager
	{
	  public:
		SceneManager();
		~SceneManager();

		// シーン切り替え
		// 引数なしのシーンにも、引数ありのシーンにも対応
		template <typename T, typename... Args>
		inline void Move(
		    Args&&... _args)
		{
			if (pMakeNextSceneFactory_)
			{
				VCU_ASSERT(false && "既にシーン遷移予定が入っている");
				return;
			}

			pMakeNextSceneFactory_ = std::make_unique<std::function<void()>>(
			    [this, args = std::make_tuple(std::forward<Args>(_args)...)]() mutable
			    {
				    pCurrentScene_ = std::apply(
				        [](auto&&... values)
				        {
					        return std::make_unique<T>(std::forward<decltype(values)>(values)...);
				        },
				        std::move(args));
			    });
		}

		inline void Clear()
		{
			if (pCurrentScene_)
			{
				pCurrentScene_->End();
				pCurrentScene_.reset();
			}
		}

		cha::SceneBase& GetCurrentScene();

		template <typename T>
		inline T& FindObject(
		    const GameObject _gameObject)
		{
			return pCurrentScene_->RefGameObjectCollection().template At<T>(_gameObject);
		}

		template <std::derived_from<IComponent> T>
		inline T* FindComponent(
		    const GameObject _gameObject)
		{
			return pCurrentScene_->RefGameObjectCollection().template FindComponent<T>(_gameObject);
		}

		template <typename... ComponentsT> inline View<ComponentsT...> GetView()
		{
			return pCurrentScene_->RefGameObjectCollection().template GetView<ComponentsT...>();
		}

		template <typename T> inline T& GetSystemObject()
		{
			return pCurrentScene_->RefGameObjectCollection().template At<T>(SystemRegistry::GetSystemIndex<T>());
		}

		GameObject MakeGameObject(const uint64_t _id);

		inline bool HasNextScene() const
		{
			return !!pMakeNextSceneFactory_;
		}

		void DestroyAt(const GameObject _gameObject);

	  private:
		bool Update() override;
		bool LateUpdate() override;
		bool Draw(const bool _isShadowing) override;
		bool Refresh() override;

	  private:
		std::unique_ptr<IScene> pCurrentScene_{};
		std::unique_ptr<std::function<void()>> pMakeNextSceneFactory_{};
	};
}  // namespace cha