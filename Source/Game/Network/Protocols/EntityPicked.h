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
			_registry.AddMember(entityId)
			    .AddMember(itemId)
			    .AddMember(positionX)
			    .AddMember(positionY)
			    .AddMember(positionZ)
			    .AddMember(pickerName);
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
		int32_t itemId{};
		float positionX{};
		float positionY{};
		float positionZ{};
		std::string pickerName{};
	};
}  // namespace Protocol