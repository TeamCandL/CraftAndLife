#pragma once
#include "../INetworkMessage.h"

namespace Protocol
{
	/// @brief プロトコル : UpdatedPlayers
	struct UpdatedPlayers : INetworkMessage
	{
		/// @brief プロトコルの種類を取得する
		/// @return プロトコルの種類
		ProtocolType GetProtocolType() const override
		{
			return ProtocolType::UpdatedPlayers;
		}

		/// @brief メンバーをロードする
		/// @param _registry メンバーを登録するやつ
		void LoadMembers(
		    MessageRegistry& _registry) override
		{
			_registry.AddMember(playerNames)
			    .AddMember(positions)
			    .AddMember(rotations)
			    .AddMember(haveItemIds)
			    .AddMember(flag);
		}

		uint8_t GetChannel() const override
		{
			return 1;
		}
		ENetSendMode GetSendMode() const override
		{
			return ENetSendMode::Unreliable;
		}

		// 以下通信メンバーたち

		std::string playerNames{};
		SizedBuffer<float> positions{};
		SizedBuffer<float> rotations{};
		SizedBuffer<int32_t> haveItemIds{};

		int32_t flag{};
	};
}  // namespace Protocol
