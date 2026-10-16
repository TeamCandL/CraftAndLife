#include "Audio.h"
#define DR_MP3_IMPLEMENTATION
#include <dr_lib/dr_mp3.h>

bool cha::AudioRegistry::TryLoad(fs::path&& _file, AudioResource& _out)
{
	// 拡張子だけ取得
	std::string ext = _file.extension().string();

	if (ext == ".mp3")
	{
		drmp3_config config{};
		drmp3_uint64 totalFrameCount = 0;

		// 最新の dr_mp3 の高レベルAPIでファイルから一括で16bit PCMをデコードする
		// 引数: (filePath, &config, &totalFrameCount, pAllocationCallbacks)
		drmp3_int16* pSampleData = drmp3_open_file_and_read_pcm_frames_s16(
			_file.string().c_str(),
			&config,
			&totalFrameCount,
			nullptr
		);

		if (!pSampleData)
		{
			return false;
		}

		// 波形フォーマット（WAVEFORMATEX）の設定
		auto& wfx = _out.waveFormatEx.Format;
		wfx.wFormatTag = WAVE_FORMAT_PCM;
		wfx.nChannels = static_cast<WORD>(config.channels);
		wfx.nSamplesPerSec = config.sampleRate;
		wfx.wBitsPerSample = 16; // s16 なので 16bit
		wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
		wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
		wfx.cbSize = 0;

		// データのサイズ（バイト数）を計算してバッファにコピー
		size_t dataSize = static_cast<size_t>(totalFrameCount * config.channels * sizeof(int16_t));
		_out.buffer.resize(dataSize);
		std::memcpy(_out.buffer.data(), pSampleData, dataSize);

		// dr_mp3 側で確保されたメモリは専用の関数で解放する
		drmp3_free(pSampleData, nullptr);

		return !_out.buffer.empty();
	}
	else if (ext == ".wav")
	{
		std::ifstream ifs{ _file, std::ios::binary };
		if (!ifs)
		{
			return false;
		}

		std::array<char, 4> chunkId{};
		uint32_t chunkSize{};

		ifs.read(chunkId.data(), chunkId.size());
		ifs.seekg(8, std::ios::cur);  // sizeと"WAVE"識別子をシーク

		while (
			ifs.read(chunkId.data(), chunkId.size())
			&& ifs.read(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize)))
		{
			if (strncmp(chunkId.data(), "fmt ", 4) == 0)
			{
				ifs.read(reinterpret_cast<char*>(&_out.waveFormatEx), std::min<size_t>(chunkSize, sizeof(_out.waveFormatEx)));
				if (chunkSize > sizeof(_out.waveFormatEx))
				{
					ifs.seekg(chunkSize - sizeof(_out.waveFormatEx), std::ios::cur);
				}
			}
			else if (strncmp(chunkId.data(), "data", 4) == 0)
			{
				_out.buffer.resize(chunkSize);
				ifs.read(reinterpret_cast<char*>(_out.buffer.data()), chunkSize);
			}
			else {
				ifs.seekg(chunkSize, std::ios::cur);
			}
		}

		return !_out.buffer.empty();
	}

	VCU_LOG("ERROR: 未対応の音声ファイル拡張子[{}]", ext);

	return false;
}

cha::HashValue cha::AudioResource::GetHash() const
{
	HashValue seed{ 0 };

	HashCombine(seed, waveFormatEx.Format.wFormatTag);
	HashCombine(seed, waveFormatEx.Format.nChannels);
	HashCombine(seed, waveFormatEx.Format.nSamplesPerSec);
	HashCombine(seed, waveFormatEx.Format.wBitsPerSample);

	// 拡張フォーマットの場合のみ追加メンバを計算
	if (waveFormatEx.Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		HashCombine(seed, waveFormatEx.Samples.wValidBitsPerSample);
		HashCombine(seed, waveFormatEx.dwChannelMask);
		// GUID (SubFormat) のハッシュ化
		// Data1, Data2, Data3, Data4をまとめて扱う
		HashCombine(seed, waveFormatEx.SubFormat.Data1);
	}

	return seed;
}
