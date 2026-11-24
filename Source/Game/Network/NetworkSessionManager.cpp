#include "NetworkSessionManager.h"
#include "MessageConverter.h"
#include "pch\pch.h"
#include "MultiMessage.h"

void NetworkSessionManager::Connect()
{
	bool succeed = transport_.Initialize();
	VCU_ASSERT(succeed && "ENetTransportの初期化に失敗");

	static const fs::path CONNECTION_JSON_FILE{ "ConnectionConfig.json" };

	json connectionJson{};
	std::ifstream ifs{ CONNECTION_JSON_FILE };

	VCU_ASSERT((ifs && ifs.is_open()) && "接続設定のjsonファイルが開けなかった");

	ifs >> connectionJson;

	std::string hostName{ connectionJson.value("ip", "127.0.0.1") };
	uint16_t portNumber{ connectionJson.value("port", 43838ui16) };

	succeed = transport_.Connect(hostName, portNumber);
	VCU_ASSERT(succeed && "サーバーへの接続に失敗");
}

void NetworkSessionManager::Update() {}

void NetworkSessionManager::Disconnect()
{
	transport_.Disconnect();
}

void NetworkSessionManager::Release()
{
	transport_.Shutdown();
}

void NetworkSessionManager::Send(
    INetworkMessage& _message)
{
	MessageConverter converter{};

	std::vector<char> binary = converter.ToBinary(_message);

	if (_message.GetSendMode() == ENetSendMode::Reliable)
	{
		sendBinaryMessages[_message.GetChannel()].push_back(binary);
	}
	else
	{
		CalServer::Utility::MultiMessagePackage package{};
		package.Add(binary);

		transport_.Send(package.Dump(), _message.GetChannel(), _message.GetSendMode());
	}

	// 送信したデータを即時にフラッシュしておく。
	// 高頻度で送信される位置情報などはENetの内部キューに溜まりやすいため
	// 明示的にフラッシュしておくことでクライアント側での遅延蓄積を防ぐ。
	// transport_.Flush();
}

void NetworkSessionManager::Flush()
{
	for (size_t i = 0; i < CHANNLEL_MAX; i++)
	{
		CalServer::Utility::MultiMessagePackage package{};

		for (auto& binary : sendBinaryMessages[i])
		{
			package.Add(binary);
		}

		if (package.GetCount() == 0)
		{
			continue;
		}

		std::vector<char> binary = package.Dump();

		transport_.Send(binary, i, ENetSendMode::Reliable);

		sendBinaryMessages[i].clear();
	}
}
