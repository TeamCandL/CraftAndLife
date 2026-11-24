#pragma once
#include "../INetworkMessage.h"

namespace Protocol
{
	/// @brief プロトコル : UpdatedPlayerRole
	struct UpdatedPlayerRole : INetworkMessage
	{
		/// @brief プロトコルの種類を取得する
		/// @return プロトコルの種類
		ProtocolType GetProtocolType() const override
		{
			return ProtocolType::UpdatedPlayerRole;
		}

		/// @brief メンバーをロードする
		/// @param _registry メンバーを登録するやつ
		void LoadMembers(
		    MessageRegistry& _registry) override
		{
			_registry.AddMember(playerId);
			_registry.AddMember(roleId);
		}

		// 以下通信メンバーたち
		int32_t playerId{};  ///< プレイヤーID
		int32_t roleId{};    ///< ジョブID
	};
}  // namespace Protocol
