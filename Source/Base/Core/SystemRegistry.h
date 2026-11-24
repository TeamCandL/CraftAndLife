#pragma once

namespace cha
{
	class NullObjectSystem;

	/// @brief システム登録
	class SystemRegistry
	{
	  private:
		SystemRegistry()  = delete;
		~SystemRegistry() = delete;

	  public:
		/// @brief システムの登録インデクスを取得する
		/// @tparam SystemT システム型
		/// @return 登録インデクス
		template <typename SystemT> inline static uint64_t GetSystemIndex()
		{
			// NullObjectSystem は必ず 0 番を使用する
			/*if constexpr (std::is_same_v<SystemT, NullObjectSystem>)
			{
			    return 0;
			}
			else*/
			{
				static const uint64_t INDEX{ ++IndexCounter() };
				return INDEX;
			}
		}

	  private:
		/// @brief 登録インデクスのカウンタを参照
		/// @return カウンタの参照ポインタ
		inline static uint64_t& IndexCounter()
		{
			static uint64_t counter{ 0 };
			return counter;
		}
	};
}  // namespace cha
