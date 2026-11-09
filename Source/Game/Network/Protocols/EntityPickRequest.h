#pragma once
#include "../INetworkMessage.h"

namespace Protocol
{
	struct EntityPickRequest : INetworkMessage
	{
		ProtocolType GetProtocolType() const override
		{
			return ProtocolType::EntityPickRequest;
		}

		void LoadMembers(
		    MessageRegistry& _registry) override
		{
			_registry.AddMember(entityId);
		}

		uint8_t GetChannel() const override
		{
			return 0;
		}
		ENetSendMode GetSendMode() const override
		{
			return ENetSendMode::Reliable;
		}

		int32_t entityId{};
	};
}  // namespace Protocol