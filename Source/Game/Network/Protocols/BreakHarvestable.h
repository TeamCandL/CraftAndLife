#pragma once
#include "../INetworkMessage.h"

namespace Protocol
{
	/// @brief プロトコル : BreakHarvestable
	struct BreakHarvestable : INetworkMessage
	{
		/// @brief プロトコルの種類を取得する
		/// @return プロトコルの種類
		ProtocolType GetProtocolType() const override
		{
			return ProtocolType::BreakHarvestable;
		}

		/// @brief メンバーをロードする
		/// @param _registry メンバーを登録するやつ
		void LoadMembers(
		    MessageRegistry& _registry) override
		{
			_registry.AddMember(breakHarvestableId);
		}

		uint8_t GetChannel() const override
		{
			return 0;
		}

		// 以下通信メンバーたち
		uint32_t breakHarvestableId;  ///< 破壊したハーベスタブルオブジェクトのId
	};
}  // namespace Protocol
