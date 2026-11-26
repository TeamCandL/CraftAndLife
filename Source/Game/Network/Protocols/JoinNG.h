#pragma once
#include "../INetworkMessage.h"

enum class JoinError : int32_t
{
	NameDuplicate = 0,
	LobbyFull     = 1
};

namespace Protocol
{
	/// @brief プロトコル : JoinNG
	struct JoinNG : INetworkMessage
	{
		/// @brief プロトコルの種類を取得する
		/// @return プロトコルの種類
		ProtocolType GetProtocolType() const override
		{
			return ProtocolType::JoinNG;
		}

		/// @brief メンバーをロードする
		/// @param _registry メンバーを登録するやつ
		void LoadMembers(
		    MessageRegistry& _registry) override
		{
			_registry.AddMember(errorTypes);
		}

		// 以下通信メンバーたち
		SizedBuffer<int32_t> errorTypes;
	};
}  // namespace Protocol
