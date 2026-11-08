#pragma once
#include "../INetworkMessage.h"

namespace Protocol
{
	struct EntityPicked : INetworkMessage
	{
		ProtocolType GetProtocolType() const override
		{
			return ProtocolType::EntityPicked;
		}

		void LoadMembers(
		    MessageRegistry& _registry) override
		{
			_registry.AddMember(entityId).AddMember(pickerName);
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
		std::string pickerName{};
	};
}  // namespace Protocol