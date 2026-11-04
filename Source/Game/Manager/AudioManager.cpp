#include "AudioManager.h"

void AudioManager::Start()
{
	XAudio2Create(pXAudio2.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR);
	pXAudio2->CreateMasteringVoice(&pMasterVoice);
}

void AudioManager::Play(
    Resource<ResourceType::Audio>& _resource)
{
	AudioResource& audio = _resource.Get();

	if (!pXAudio2)
	{
		return;
	}

	IXAudio2SourceVoice* pSourceVoice = nullptr;
	HRESULT hResult                   = pXAudio2->CreateSourceVoice(&pSourceVoice, &audio.waveFormatEx.Format);
	if (FAILED(hResult) || pSourceVoice == nullptr)
	{
		return;  // 失敗
	}

	const XAUDIO2_BUFFER BUFFER{
		.Flags      = XAUDIO2_END_OF_STREAM,
		.AudioBytes = static_cast<UINT32>(audio.buffer.size()),
		.pAudioData = audio.buffer.data(),
		.PlayBegin  = 0,
		.PlayLength = 0,
		.LoopBegin  = 0,
		.LoopLength = 0,
		.LoopCount  = 0,
		.pContext   = 0,
	};

	hResult = pSourceVoice->SubmitSourceBuffer(&BUFFER);
	if (FAILED(hResult))
	{
		pSourceVoice->DestroyVoice();
		return;
	}

	hResult = pSourceVoice->Start();
	if (FAILED(hResult))
	{
		pSourceVoice->DestroyVoice();
		return;
	}

	// AudioPlayer を作成して管理リストに追加
	players.emplace_back(pSourceVoice, _resource);
	players.back().Play(_resource);
}

// 足音を再生
void AudioManager::PlayFootstep(
    Resource<ResourceType::Audio>& _resource)
{
	AudioResource& audio = _resource.Get();

	if (!pXAudio2)
	{
		return;
	}

	// 前回の足音が残っていたら破棄
	StopFootstep();

	// 音声ソースを作成
	HRESULT hResult = pXAudio2->CreateSourceVoice(&pFootstepSourceVoice, &audio.waveFormatEx.Format);

	if (FAILED(hResult) || pFootstepSourceVoice == nullptr)
	{
		return;
	}

	const XAUDIO2_BUFFER BUFFER{
		.Flags      = XAUDIO2_END_OF_STREAM,
		.AudioBytes = static_cast<UINT32>(audio.buffer.size()),
		.pAudioData = audio.buffer.data(),
		.PlayBegin  = 0,
		.PlayLength = 0,
		.LoopBegin  = 0,
		.LoopLength = 0,
		.LoopCount  = 0,
		.pContext   = 0,
	};

	hResult = pFootstepSourceVoice->SubmitSourceBuffer(&BUFFER);

	if (FAILED(hResult))
	{
		StopFootstep();
		return;
	}

	hResult = pFootstepSourceVoice->Start();

	if (FAILED(hResult))
	{
		StopFootstep();
		return;
	}
}

// 足音を停止
void AudioManager::StopFootstep()
{
	if (pFootstepSourceVoice)
	{
		pFootstepSourceVoice->Stop();
		pFootstepSourceVoice->DestroyVoice();
		pFootstepSourceVoice = nullptr;
	}
}

void AudioManager::LateUpdate(
    const float _deltaTime)
{
	for (size_t i = 0; i < players.size();)
	{
		players[i].timeLeftSec -= _deltaTime;
		if (players[i].timeLeftSec <= 0.0f)
		{
			if (players[i].pSourceVoice)
			{
				players[i].pSourceVoice->Stop();
				players[i].pSourceVoice->DestroyVoice();
				players[i].pSourceVoice = nullptr;
			}
			// erase by swapping with last for O(1) removal
			if (i + 1 < players.size())
			{
				players[i] = std::move(players.back());
			}
			players.pop_back();
		}
		else
		{
			++i;
		}
	}
}

void AudioManager::Update() {}

void AudioManager::SetBGM(
    Resource<ResourceType::Audio>& _resource)
{
	AudioResource& audio = _resource.Get();

	if (pBGMSourceVoice)
	{
		pBGMSourceVoice->Stop();
		pBGMSourceVoice->DestroyVoice();
		pBGMSourceVoice = nullptr;
	}

	HRESULT hResult{};

	hResult = pXAudio2->CreateSourceVoice(&pBGMSourceVoice, &audio.waveFormatEx.Format);
	VCU_ASSERT(SUCCEEDED(hResult) && "BGM用SourceVoiceの作成に失敗");
	if (FAILED(hResult))
	{
		return;  // 失敗
	}

	const XAUDIO2_BUFFER BUFFER{
		.Flags      = XAUDIO2_END_OF_STREAM,
		.AudioBytes = static_cast<UINT32>(audio.buffer.size()),
		.pAudioData = audio.buffer.data(),
		.PlayBegin  = 0,
		.PlayLength = 0,
		.LoopBegin  = 0,
		.LoopLength = 0,
		.LoopCount  = XAUDIO2_LOOP_INFINITE,
		.pContext   = 0,
	};

	hResult = pBGMSourceVoice->SubmitSourceBuffer(&BUFFER);
	VCU_ASSERT(SUCCEEDED(hResult) && "BGM用バッファ登録に失敗");
	if (FAILED(hResult))
	{
		return;  // 失敗
	}

	hResult = pBGMSourceVoice->Start();
	VCU_ASSERT(SUCCEEDED(hResult) && "BGMの再生に失敗");
	if (FAILED(hResult))
	{
		return;  // 失敗
	}
}

void AudioManager::SetBGMExitLoop()
{
	if (pBGMSourceVoice)
	{
		pBGMSourceVoice->ExitLoop();
	}
}

void AudioManager::SetBGMStop()
{
	if (pBGMSourceVoice)
	{
		pBGMSourceVoice->Stop();
	}
}

void AudioManager::SetBGMStart()
{
	if (pBGMSourceVoice)
	{
		pBGMSourceVoice->Start();
	}
}

void AudioManager::Release()
{
	// 足音を停止
	StopFootstep();

	if (pBGMSourceVoice)
	{
		pBGMSourceVoice->Stop();
		pBGMSourceVoice->DestroyVoice();
		pBGMSourceVoice = nullptr;
	}

	if (pMasterVoice)
	{
		pMasterVoice->DestroyVoice();
		pMasterVoice = nullptr;
	}

	// 再生中の全てのソースボイスを停止して破棄
	for (auto& player : players)
	{
		if (player.pSourceVoice)
		{
			player.pSourceVoice->Stop();
			player.pSourceVoice->DestroyVoice();
			player.pSourceVoice = nullptr;
		}
	}
	players.clear();
}
