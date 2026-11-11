#pragma once
#include "../INetworkMessage.h"

namespace Protocol
{
	struct PlayerRespawned : INetworkMessage
	{
		ProtocolType GetProtocolType() const override
		{
			return ProtocolType::PlayerRespawned;
		}

		void LoadMembers(
		    MessageRegistry& _registry) override
		{
			_registry.AddMember(hp).AddMember(playerName);
		}

		uint8_t GetChannel() const override
		{
			return 0;
		}
		ENetSendMode GetSendMode() const override
		{
			return ENetSendMode::Reliable;
		}

		int32_t hp{};
		std::string playerName{};
	};
}  // namespace Protocol