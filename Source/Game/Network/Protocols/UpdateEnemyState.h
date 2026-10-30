#pragma once
#include "../INetworkMessage.h"

namespace Protocol
{
	/// @brief プロトコル : UpdateEnemyState
	struct UpdateEnemyState : INetworkMessage
	{
		ProtocolType GetProtocolType() const override
		{
			return ProtocolType::UpdateEnemyState;
		}

		void LoadMembers(
		    MessageRegistry& _registry) override
		{
			_registry.AddMember(enemyId).AddMember(stateName).AddMember(value);
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
		std::string stateName{};
		bool value{};
	};
}  // namespace Protocol