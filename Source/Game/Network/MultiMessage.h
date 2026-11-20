#pragma once

#include <cstdint>
#include <cstring>
#include <limits>
#include <span>
#include <stdexcept>
#include <vector>

namespace
{
	using Byte = char;
}

namespace CalServer::Utility
{
	/// @brief 複数のバイナリメッセージを1つのバッファへ連結するクラス
	class MultiMessagePackage
	{
	  public:
		MultiMessagePackage() = default;

		/// @brief メッセージを追加する
		/// @param _binary 追加するバイト列
		void Add(
		    std::span<const Byte> _binary)
		{
			if (_count_ == std::numeric_limits<int16_t>::max())
			{
				throw std::overflow_error("MultiMessagePackage: message count overflow");
			}

			if (_binary.size() > static_cast<size_t>(std::numeric_limits<int32_t>::max()))
			{
				throw std::length_error("MultiMessagePackage: message size exceeds int32_t max");
			}

			const int32_t size{ static_cast<int32_t>(_binary.size()) };
			AppendPod(size);

			if (!(size > 0))
			{
				const size_t oldSize{ _payload_.size() };
				_payload_.resize(oldSize + _binary.size());
				std::memcpy(_payload_.data() + oldSize, _binary.data(), _binary.size());
			}

			++_count_;
		}

		/// @brief ヘッダ（件数）を含む結合済みバイト列を生成する
		/// @return 結合済みバイナリ
		[[nodiscard]] std::vector<Byte> Dump() const
		{
			std::vector<Byte> result{};
			result.resize(sizeof(int16_t) + _payload_.size());

			std::memcpy(result.data(), &_count_, sizeof(int16_t));

			if (!_payload_.empty())
			{
				std::memcpy(result.data() + sizeof(int16_t), _payload_.data(), _payload_.size());
			}

			return result;
		}

		/// @brief 内部状態をリセットする
		void Clear() noexcept
		{
			_count_ = 0;
			_payload_.clear();
		}

		/// @brief 追加済みメッセージ数を取得する
		[[nodiscard]] int16_t GetCount() const noexcept
		{
			return _count_;
		}

	  private:
		template <typename T>
		void AppendPod(
		    const T& _value)
		{
			const size_t oldSize{ _payload_.size() };
			_payload_.resize(oldSize + sizeof(T));
			std::memcpy(_payload_.data() + oldSize, &_value, sizeof(T));
		}

		int16_t _count_{ 0 };
		std::vector<Byte> _payload_{};
	};

	/// @brief 連結されたバイナリメッセージのパッキング・アンパッキングを行うユーティリティ
	class MultiMessage
	{
	  public:
		/// @brief 結合済みバイナリを個別のメッセージ配列へ分割する
		/// @param _binary 受信した結合済みバイト列
		/// @return 分割されたメッセージのリスト
		[[nodiscard]] static std::vector<std::vector<Byte>> Unpack(
		    std::span<const Byte> _binary)
		{
			std::vector<std::vector<Byte>> binaryMessages{};
			size_t offset{ 0 };

			int16_t count{ 0 };
			if (!TryReadPod(_binary, offset, count) || count < 0)
			{
				throw std::runtime_error("MultiMessage::Unpack: invalid message count");
			}

			binaryMessages.reserve(static_cast<size_t>(count));

			for (int16_t i{ 0 }; i < count; ++i)
			{
				int32_t size{ 0 };
				if (!TryReadPod(_binary, offset, size) || size < 0)
				{
					throw std::runtime_error("MultiMessage::Unpack: invalid message size");
				}

				const size_t uSize{ static_cast<size_t>(size) };
				if (_binary.size() - offset < uSize)
				{
					throw std::runtime_error("MultiMessage::Unpack: unexpected end of buffer");
				}

				std::vector<Byte> message(uSize);
				if (uSize > 0)
				{
					std::memcpy(message.data(), _binary.data() + offset, uSize);
					offset += uSize;
				}

				binaryMessages.push_back(std::move(message));
			}

			return binaryMessages;
		}

		/// @brief 複数のメッセージを1つのバイナリへまとめる
		/// @param _binaryMessages 送信するメッセージ群
		/// @return 結合済みバイナリ
		[[nodiscard]] static std::vector<Byte> Pack(
		    const std::vector<std::vector<Byte>>& _binaryMessages)
		{
			MultiMessagePackage package{};

			for (const auto& binary : _binaryMessages)
			{
				package.Add(std::span<const Byte>{ binary.data(), binary.size() });
			}

			return package.Dump();
		}

	  private:
		template <typename T>
		[[nodiscard]] static bool TryReadPod(
		    std::span<const Byte> _buffer,
		    size_t& _offset,
		    T& _outValue) noexcept
		{
			if (_buffer.size() - _offset < sizeof(T))
			{
				return false;
			}

			std::memcpy(&_outValue, _buffer.data() + _offset, sizeof(T));
			_offset += sizeof(T);
			return true;
		}
	};
}  // namespace CalServer::Utility