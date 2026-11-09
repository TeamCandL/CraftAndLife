#pragma once
#include "../INetworkMessage.h"

namespace Protocol
{
	struct EntitySpawnRequest : INetworkMessage
	{
		ProtocolType GetProtocolType() const override
		{
			return ProtocolType::EntitySpawnRequest;
		}

		void LoadMembers(
		    MessageRegistry& _registry) override
		{
			_registry.AddMember(itemId)
			    .AddMember(positionX)
			    .AddMember(positionY)
			    .AddMember(positionZ)
			    .AddMember(velocityX)
			    .AddMember(velocityY)
			    .AddMember(velocityZ);
		}

		uint8_t GetChannel() const override
		{
			return 0;
		}
		ENetSendMode GetSendMode() const override
		{
			return ENetSendMode::Reliable;
		}

		int32_t itemId{};
		float positionX{}, positionY{}, positionZ{};
		float velocityX{}, velocityY{}, velocityZ{};
	};
}  // namespace Protocol