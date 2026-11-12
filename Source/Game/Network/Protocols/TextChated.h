#pragma once
#include "../INetworkMessage.h"

namespace Protocol
{
	/// @brief プロトコル : TextChated
	struct TextChated : INetworkMessage
	{
		/// @brief プロトコルの種類を取得する
		/// @return プロトコルの種類
		ProtocolType GetProtocolType() const override
		{
			return ProtocolType::TextChated;
		}

		/// @brief メンバーをロードする
		/// @param _registry メンバーを登録するやつ
		void LoadMembers(
		    MessageRegistry& _registry) override
		{
			_registry.AddMember(content).AddMember(sender);
		}

		// 以下通信メンバーたち
		std::string content{};
		std::string sender{};
	};
}  // namespace Protocol
