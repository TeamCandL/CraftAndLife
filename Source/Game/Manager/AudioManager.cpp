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

	StopFootstep();

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

// ジャンプ音を再生
void AudioManager::PlayJump(
    Resource<ResourceType::Audio>& _resource)
{
	AudioResource& audio = _resource.Get();

	if (!pXAudio2)
	{
		return;
	}

	// 初回だけSourceVoiceを作成
	if (!pJumpSourceVoice)
	{
		HRESULT hResult = pXAudio2->CreateSourceVoice(&pJumpSourceVoice, &audio.waveFormatEx.Format);

		if (FAILED(hResult) || pJumpSourceVoice == nullptr)
		{
			return;
		}
	}

	// 再生中なら停止してバッファをクリア
	pJumpSourceVoice->Stop();
	pJumpSourceVoice->FlushSourceBuffers();

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

	HRESULT hResult = pJumpSourceVoice->SubmitSourceBuffer(&BUFFER);

	if (FAILED(hResult))
	{
		return;
	}

	hResult = pJumpSourceVoice->Start();

	if (FAILED(hResult))
	{
		return;
	}
}

// ジャンプ音を停止
void AudioManager::StopJump()
{
	if (pJumpSourceVoice)
	{
		pJumpSourceVoice->Stop();
		pJumpSourceVoice->FlushSourceBuffers();
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

// BGMを再生
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
		.LoopCount  = XAUDIO2_LOOP_INFINITE,
		.pContext   = 0,
	};

	hResult = pBGMSourceVoice->SubmitSourceBuffer(&BUFFER);

	VCU_ASSERT(SUCCEEDED(hResult) && "BGM用バッファ登録に失敗");

	if (FAILED(hResult))
	{
		return;
	}

	hResult = pBGMSourceVoice->Start();

	VCU_ASSERT(SUCCEEDED(hResult) && "BGMの再生に失敗");

	if (FAILED(hResult))
	{
		return;
	}
}

// 環境音をループ再生
void AudioManager::SetAmbientSound(
    Resource<ResourceType::Audio>& _resource)
{
	AudioResource& audio = _resource.Get();

	if (!pXAudio2)
	{
		return;
	}

	// すでに再生中の環境音があれば停止
	StopAmbientSound();

	HRESULT hResult = pXAudio2->CreateSourceVoice(&pAmbientSourceVoice, &audio.waveFormatEx.Format);

	if (FAILED(hResult) || pAmbientSourceVoice == nullptr)
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
		.LoopCount  = XAUDIO2_LOOP_INFINITE,
		.pContext   = 0,
	};

	hResult = pAmbientSourceVoice->SubmitSourceBuffer(&BUFFER);

	if (FAILED(hResult))
	{
		StopAmbientSound();
		return;
	}

	hResult = pAmbientSourceVoice->Start();

	if (FAILED(hResult))
	{
		StopAmbientSound();
		return;
	}
}

// 環境音を停止
void AudioManager::StopAmbientSound()
{
	if (pAmbientSourceVoice)
	{
		pAmbientSourceVoice->Stop();
		pAmbientSourceVoice->DestroyVoice();
		pAmbientSourceVoice = nullptr;
	}
}

// BGMのループを終了
void AudioManager::SetBGMExitLoop()
{
	if (pBGMSourceVoice)
	{
		pBGMSourceVoice->ExitLoop();
	}
}

// BGMを停止
void AudioManager::SetBGMStop()
{
	if (pBGMSourceVoice)
	{
		pBGMSourceVoice->Stop();
	}
}

// BGMを再開
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

	// ジャンプ音を停止・破棄
	if (pJumpSourceVoice)
	{
		pJumpSourceVoice->Stop();
		pJumpSourceVoice->DestroyVoice();
		pJumpSourceVoice = nullptr;
	}

	// 環境音を停止
	StopAmbientSound();

	// BGMを停止
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
