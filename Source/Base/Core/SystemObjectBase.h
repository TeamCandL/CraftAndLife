#pragma once
#include "ObjectBase.h"
#include "SystemRegistry.h"
#include "NullObjectData.h"
#include "ISystemObject.h"
#include <AllSystem.h>

namespace cha
{
	/// @brief システムオブジェクトの基底クラス
	/// @tparam SystemT システムの型
	/// @tparam DataT データ構造体
	template <typename SystemT, typename DataT = NullObjectData>
	class SystemObjectBase : public ObjectBase<DataT>, public ISystemObject
	{
	  public:
		/// @brief 参照関係を登録、リンクする
		/// @param _linker リンクするやつ
		virtual void OnDependencyLink(
		    DependencyLinker& _linker) override
		{
		}

		/// @brief 遅い更新処理
		void LateUpdate() override {}

	  private:
		static void RegisterAtStartup()
		{
			SystemRegistry::GetSystemIndex<SystemT>();
		}

		static constexpr size_t INDEX = AllSystems::IndexOf<SystemT>();
		static_assert(AllSystems::Contains<SystemT>(), "AllSystems に登録されていないシステムです");
	};
}  // namespace cha
