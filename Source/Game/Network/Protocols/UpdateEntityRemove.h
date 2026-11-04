#pragma once
#include "../INetworkMessage.h"

namespace Protocol
{
	/// @brief プロトコル : UpdateEntityRemove
	struct UpdateEntityRemove : INetworkMessage
	{
		ProtocolType GetProtocolType() const override
		{
			return ProtocolType::UpdateEntityRemove;
		}

		void LoadMembers(
		    MessageRegistry& _registry) override
		{
			_registry.AddMember(entityId);
		}

		uint8_t GetChannel() const override
		{
			return 1;
		}

		ENetSendMode GetSendMode() const override
		{
			return ENetSendMode::Reliable;
		}

		// 通信メンバー
		int32_t entityId{};
	};
}  // namespace Protocol