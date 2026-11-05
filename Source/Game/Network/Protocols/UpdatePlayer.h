#pragma once
#include "../INetworkMessage.h"

namespace Protocol
{
	/// @brief プロトコル : UpdatePlayer
	struct UpdatePlayer : INetworkMessage
	{
		/// @brief プロトコルの種類を取得する
		/// @return プロトコルの種類
		ProtocolType GetProtocolType() const override
		{
			return ProtocolType::UpdatePlayer;
		}

		/// @brief メンバーをロードする
		/// @param _registry メンバーを登録するやつ
		void LoadMembers(
		    MessageRegistry& _registry) override
		{
			_registry.AddMember(positionX)
			    .AddMember(positionY)
			    .AddMember(positionZ)

			    .AddMember(rotationX)
			    .AddMember(rotationY)

			    .AddMember(haveItemId)
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
		float positionX{};  ///< 座標(x)
		float positionY{};  ///< 座標(y)
		float positionZ{};  ///< 座標(z)

		float rotationX{};  ///< 腕の回転(x)
		float rotationY{};  ///< ボディの回転(y)

		int32_t haveItemId{};  ///< 持っているアイテムのId

		int32_t flag{};  ///< フラグ
	};
}  // namespace Protocol
