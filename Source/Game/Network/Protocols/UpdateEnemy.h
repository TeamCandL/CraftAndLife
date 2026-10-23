#pragma once
#include "../INetworkMessage.h"

namespace Protocol
{
	/// @brief プロトコル : UpdateEnemy
	struct UpdateEnemy : INetworkMessage
	{
		/// @brief プロトコルの種類を取得する
		/// @return プロトコルの種類
		ProtocolType GetProtocolType() const override
		{
			return ProtocolType::UpdateEnemy;
		}

		/// @brief メンバーをロードする
		/// @param _registry メンバーを登録するやつ
		void LoadMembers(
		    MessageRegistry& _registry) override
		{
			_registry.AddMember(enemyId)
			    .AddMember(enemyType)

			    .AddMember(positionX)
			    .AddMember(positionY)
			    .AddMember(positionZ)

			    .AddMember(rotationX)
			    .AddMember(rotationY)

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
		int enemyId{};
		int enemyType{};

		float positionX{};
		float positionY{};
		float positionZ{};

		float rotationX{};
		float rotationY{};

		int flag{};
	};
}  // namespace Protocol
