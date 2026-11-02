#pragma once
#include "../INetworkMessage.h"

namespace Protocol
{
	struct UpdateEntitySpawn : INetworkMessage
	{
		ProtocolType GetProtocolType() const override
		{
			return ProtocolType::UpdateEntitySpawn;
		}

		void LoadMembers(
		    MessageRegistry& _registry) override
		{
			_registry.AddMember(entityId)
			    .AddMember(itemId)
			    .AddMember(sourceType)
			    .AddMember(sourceId)
			    .AddMember(positionX)
			    .AddMember(positionY)
			    .AddMember(positionZ)
			    .AddMember(velocityX)
			    .AddMember(velocityY)
			    .AddMember(velocityZ)
			    .AddMember(localEntityId)
			    .AddMember(requestPlayerId);
		}

		uint8_t GetChannel() const override
		{
			return 1;
		}
		ENetSendMode GetSendMode() const override
		{
			return ENetSendMode::Reliable;
		}

		int32_t entityId{};
		int32_t itemId{};
		uint8_t sourceType{};
		int32_t sourceId{};

		float positionX{}, positionY{}, positionZ{};
		float velocityX{}, velocityY{}, velocityZ{};

		int32_t localEntityId{};
		uint32_t requestPlayerId{};
	};
}  // namespace Protocol