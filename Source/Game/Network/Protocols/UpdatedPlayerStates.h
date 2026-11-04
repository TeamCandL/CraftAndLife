#pragma once
#include "../INetworkMessage.h"

namespace Protocol
{
	/// @brief プロトコル : UpdatedPlayerStates
	struct UpdatedPlayerStates : INetworkMessage
	{
		ProtocolType GetProtocolType() const override
		{
			return ProtocolType::UpdatedPlayerStates;
		}

		void LoadMembers(
		    MessageRegistry& _registry) override
		{
			_registry.AddMember(playerId).AddMember(stateName).AddMember(values);
		}

		uint8_t GetChannel() const override
		{
			return 0;
		}

		ENetSendMode GetSendMode() const override
		{
			return ENetSendMode::Reliable;
		}

		// 通信メンバー
		int32_t playerId{};
		std::string stateName{};
		SizedBuffer<uint8_t> values{};
	};
}  // namespace Protocol