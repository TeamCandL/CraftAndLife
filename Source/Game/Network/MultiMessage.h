
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
			// メッセージ数の上限チェック
			if (_count_ >= std::numeric_limits<int16_t>::max())
			{
				throw std::overflow_error("MultiMessagePackage: message count overflow");
			}

			// メッセージサイズの上限チェック
			if (_binary.size() > static_cast<size_t>(std::numeric_limits<int32_t>::max()))
			{
				throw std::length_error("MultiMessagePackage: message size exceeds int32_t max");
			}

			const int32_t size = static_cast<int32_t>(_binary.size());

			// バッファサイズのオーバーフローチェック
			const size_t oldSize = _payload_.size();
			const size_t maxSize = _payload_.max_size();

			if (oldSize > maxSize - sizeof(int32_t) || _binary.size() > maxSize - oldSize - sizeof(int32_t))
			{
				throw std::length_error("MultiMessagePackage: payload size overflow");
			}

			// サイズ情報 + メッセージ本体の領域を確保
			const size_t newSize = oldSize + sizeof(int32_t) + _binary.size();

			_payload_.resize(newSize);

			// サイズ情報を書き込む
			std::memcpy(_payload_.data() + oldSize, &size, sizeof(int32_t));

			// メッセージ本体を書き込む
			if (!_binary.empty())
			{
				std::memcpy(_payload_.data() + oldSize + sizeof(int32_t), _binary.data(), _binary.size());
			}

			++_count_;
		}

		/// @brief ヘッダ（件数）を含む結合済みバイト列を生成する
		/// @return 結合済みバイナリ
		[[nodiscard]]
		std::vector<Byte> Dump() const
		{
			std::vector<Byte> result{};

			result.resize(sizeof(int16_t) + _payload_.size());

			// メッセージ数
			std::memcpy(result.data(), &_count_, sizeof(int16_t));

			// メッセージデータ
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
		[[nodiscard]]
		int16_t GetCount() const noexcept
		{
			return _count_;
		}

	  private:
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
		[[nodiscard]]
		static std::vector<std::vector<Byte>> Unpack(
		    std::span<const Byte> _binary)
		{
			std::vector<std::vector<Byte>> binaryMessages{};

			size_t offset = 0;

			// メッセージ数を読み込む
			int16_t count = 0;

			if (!TryReadPod(_binary, offset, count) || count < 0)
			{
				throw std::runtime_error("MultiMessage::Unpack: invalid message count");
			}

			binaryMessages.reserve(static_cast<size_t>(count));

			// メッセージ数分だけ読み込む
			for (int16_t i = 0; i < count; ++i)
			{
				// メッセージサイズを読み込む
				int32_t size = 0;

				if (!TryReadPod(_binary, offset, size) || size < 0)
				{
					throw std::runtime_error("MultiMessage::Unpack: invalid message size");
				}

				const size_t uSize = static_cast<size_t>(size);

				// 残りのバッファサイズを確認
				if (offset > _binary.size() || uSize > _binary.size() - offset)
				{
					throw std::runtime_error("MultiMessage::Unpack: unexpected end of buffer");
				}

				// メッセージ本体をコピー
				std::vector<Byte> message(uSize);

				if (uSize > 0)
				{
					std::memcpy(message.data(), _binary.data() + offset, uSize);
				}

				offset += uSize;

				binaryMessages.push_back(std::move(message));
			}

			// 余分なデータが残っていないか確認
			if (offset != _binary.size())
			{
				throw std::runtime_error("MultiMessage::Unpack: unexpected trailing data");
			}

			return binaryMessages;
		}

		/// @brief 複数のメッセージを1つのバイナリへまとめる
		/// @param _binaryMessages 送信するメッセージ群
		/// @return 結合済みバイナリ
		[[nodiscard]]
		static std::vector<Byte> Pack(
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
		/// @brief バッファから指定型の値を読み込む
		template <typename T>
		[[nodiscard]]
		static bool TryReadPod(
		    std::span<const Byte> _buffer,
		    size_t& _offset,
		    T& _outValue) noexcept
		{
			if (_offset > _buffer.size() || sizeof(T) > _buffer.size() - _offset)
			{
				return false;
			}

			std::memcpy(&_outValue, _buffer.data() + _offset, sizeof(T));

			_offset += sizeof(T);

			return true;
		}
	};

}  // namespace CalServer::Utility
