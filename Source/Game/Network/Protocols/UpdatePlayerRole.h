#pragma once
#include "../INetworkMessage.h"

namespace Protocol
{
	/// @brief プロトコル : UpdatePlayerRole
	struct UpdatePlayerRole : INetworkMessage
	{
		/// @brief プロトコルの種類を取得する
		/// @return プロトコルの種類
		ProtocolType GetProtocolType() const override
		{
			return ProtocolType::UpdatePlayerRole;
		}

		/// @brief メンバーをロードする
		/// @param _registry メンバーを登録するやつ
		void LoadMembers(
		    MessageRegistry& _registry) override
		{
			_registry.AddMember(roleId);
		}

		// 以下通信メンバーたち
		int32_t roleId{};  ///< ジョブID
	};
}  // namespace Protocol
