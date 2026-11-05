#pragma once
#include "../INetworkMessage.h"

namespace Protocol
{
	struct UpdateEnemyDamage : INetworkMessage
	{
		ProtocolType GetProtocolType() const override
		{
			return ProtocolType::UpdateEnemyDamage;
		}

		void LoadMembers(
		    MessageRegistry& registry) override
		{
			registry.AddMember(enemyId).AddMember(damage);
		}

		uint8_t GetChannel() const override
		{
			return 1;
		}

		ENetSendMode GetSendMode() const override
		{
			return ENetSendMode::Reliable;
		}

		int32_t enemyId{};
		int32_t damage{};
	};
}  // namespace Protocol