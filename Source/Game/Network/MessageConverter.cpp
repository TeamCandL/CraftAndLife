#include "MessageConverter.h"
#include "MessageRegistry.h"
#include "pch\pch.h"

std::vector<char> MessageConverter::ToBinary(INetworkMessage& _message)
{
	std::vector<char> buffer{};

	buffer.push_back(static_cast<uint8_t>(_message.GetProtocolType()));
	size_t pos{1};

	MessageRegistry registry{};
	_message.LoadMembers(registry);

	for (auto& value : registry.members)
	{
		std::visit(
		    [&buffer](auto* _arg)
		    {
			    using T = std::decay_t<decltype(*_arg)>;

			    // 指定したサイズ分をバッファに書き込む
			    auto writeToBuffer = [&buffer, &_arg](const size_t _vSize)
			    {
				    size_t oldBufferSize = buffer.size();
				    buffer.resize(buffer.size() + _vSize, 0x00);
				    std::memcpy(buffer.data() + oldBufferSize, _arg, _vSize);
			    };

			    if constexpr (std::is_same_v<T, std::string>)
			    {
				    // _arg 自体が std::string&
				    const size_t vSize = _arg->size() + 1; // null文字含める

				    size_t oldBufferSize = buffer.size();
				    buffer.resize(oldBufferSize + vSize, 0x00);

				    // c_str() を使って null文字まで安全にコピー
				    std::memcpy(buffer.data() + oldBufferSize, _arg->c_str(), vSize);
			    }
			    else if constexpr (std::is_base_of_v<ISizedBuffer, T>)
			    {
				    // 先頭にデータサイズ (size_t) を付与して追記
				    const int32_t payloadSize = static_cast<int32_t>(_arg->ByteSize());
				    const size_t totalSize = payloadSize + sizeof(int32_t);

				    size_t oldBufferSize = buffer.size();
				    buffer.resize(oldBufferSize + totalSize, 0x00);

				    // 1. サイズ情報の書き込み
				    std::memcpy(buffer.data() + oldBufferSize, &payloadSize, sizeof(int32_t));

				    // 2. 実データの書き込み
				    if (payloadSize > 0)
				    {
					    std::memcpy(buffer.data() + oldBufferSize + sizeof(int32_t), _arg->Data(), payloadSize);
				    }
			    }
			    else
			    {
				    writeToBuffer(sizeof(T));
			    }
		    },
		    value);
	}

	return buffer;
}

void MessageConverter::ToMessage(const std::span<const char> _binary, INetworkMessage& _message)
{
	VCU_ASSERT((_message.GetProtocolType() == GetProtocolType(_binary)) && "プロトコルタイプのミスマッチが起きている");

	MessageRegistry registry{};
	_message.LoadMembers(registry);

	size_t pos{1};

	for (auto& value : registry.members)
	{
		std::visit(
		    [&_binary, &pos](auto* _arg)
		    {
			    using T = std::decay_t<decltype(*_arg)>;

			    // 指定したサイズ分をバッファに書き込む
			    auto readFromBuffer = [&_binary, &pos, &_arg](const size_t _vSize) -> bool
			    {
				    if (pos + _vSize > _binary.size())
				    {
					    VCU_LOG(
					        "MessageConverter::ToMessage: バイナリサイズ不足 expected={} remaining={}", _vSize,
					        _binary.size() - pos);
					    return false;
				    }
				    std::memcpy(_arg, &_binary[pos], _vSize);
				    pos += _vSize;
				    return true;
			    };

			    if constexpr (std::is_same_v<T, std::string>)
			    {
				    auto itr = std::find(_binary.begin() + pos, _binary.end(), '\0');

				    // null終端が見つからない場合は残り全部を文字列として扱う
				    if (itr == _binary.end())
				    {
					    size_t strLen = _binary.size() - pos;
					    _arg->assign(&_binary[pos], strLen);
					    pos = _binary.size();
				    }
				    else
				    {
					    size_t strLen = std::distance(_binary.begin() + pos, itr);
					    _arg->assign(&_binary[pos], strLen);
					    pos += strLen + 1; // null文字分もスキップ
				    }
			    }
			    else if constexpr (std::is_base_of_v<ISizedBuffer, T>)
			    {
				    // 1. サイズ情報を読み込む
				    const int32_t headerSize = static_cast<int32_t>(sizeof(int32_t));
				    if (pos + headerSize > _binary.size())
				    {
					    VCU_LOG(
					        "MessageConverter::ToMessage: ISizedBuffer ヘッダが不足しています pos={} total={}", pos,
					        _binary.size());
					    return;
				    }

				    int32_t payloadSize = 0;
				    std::memcpy(&payloadSize, &_binary[pos], headerSize);
				    pos += headerSize;

				    if (payloadSize < 0)
				    {
					    VCU_LOG(
					        "MessageConverter::ToMessage: ISizedBuffer のペイロードサイズ不正 payloadSize={}",
					        payloadSize);
					    return;
				    }

				    const size_t requiredBytes = static_cast<size_t>(payloadSize);

				    // バイナリが不足している場合は、読み取れる分だけ読み取って警告を出す
				    const size_t available = (_binary.size() >= pos) ? (_binary.size() - pos) : 0u;
				    const size_t toRead = std::min(requiredBytes, available);

				    _arg->ResizeBytes(requiredBytes);

				    if (toRead > 0)
				    {
					    std::memcpy(_arg->Data(), &_binary[pos], toRead);
					    pos += toRead;
				    }

				    if (toRead != requiredBytes)
				    {
					    VCU_LOG(
					        "MessageConverter::ToMessage: ISizedBuffer の読み取りが部分的でした required={} read={}",
					        requiredBytes, toRead);
					    return;
				    }
			    }
			    else
			    {
				    if (!readFromBuffer(sizeof(T)))
				    {
					    return;
				    }
			    }
		    },
		    value);
	}
}

ProtocolType MessageConverter::GetProtocolType(const std::span<const char> _binary)
{
	if (_binary.size() == 0)
	{
		return ProtocolType::Idk; // バイナリが空っぽ
	}

	const char HEAD_BYTE = _binary.front();
	return static_cast<ProtocolType>(HEAD_BYTE);
}
