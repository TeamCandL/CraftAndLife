#pragma once
#include "../INetworkMessage.h"

namespace Protocol
{
	/// @brief プロトコル : UpdatedEnemyStates
	struct UpdatedEnemyStates : INetworkMessage
	{
		/// @brief プロトコルの種類を取得する
		/// @return プロトコルの種類
		ProtocolType GetProtocolType() const override
		{
			return ProtocolType::UpdatedEnemyStates;
		}

		/// @brief メンバーをロードする
		/// @param _registry メンバーを登録するやつ
		void LoadMembers(
		    MessageRegistry& _registry) override
		{
			_registry.AddMember(enemyStateIds).AddMember(stateName).AddMember(values).AddMember(isFullSync);
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

		SizedBuffer<int32_t> enemyStateIds{};
		std::string stateName{};
		SizedBuffer<bool> values{};  /// 修正必要かもしれない
		bool isFullSync{};
	};
}  // namespace Protocol
