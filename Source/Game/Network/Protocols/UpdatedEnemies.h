#pragma once
#include "../INetworkMessage.h"

namespace Protocol
{
	/// @brief プロトコル : UpdatedEnemies
	struct UpdatedEnemies : INetworkMessage
	{
		/// @brief プロトコルの種類を取得する
		/// @return プロトコルの種類
		ProtocolType GetProtocolType() const override
		{
			return ProtocolType::UpdatedEnemies;
		}

		/// @brief メンバーをロードする
		/// @param _registry メンバーを登録するやつ
		void LoadMembers(
		    MessageRegistry& _registry) override
		{
			_registry.AddMember(enemyIds)
			    .AddMember(enemyTypes)
			    .AddMember(positions)
			    .AddMember(rotations)
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

		SizedBuffer<int> enemyIds{};
		SizedBuffer<int> enemyTypes{};
		SizedBuffer<float> positions{};
		SizedBuffer<float> rotations{};

		int32_t flag{};
	};
}  // namespace Protocol
