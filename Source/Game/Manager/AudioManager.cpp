#include "AudioManager.h"
#include <algorithm>

void AudioManager::Start()
{
	XAudio2Create(pXAudio2.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR);
	pXAudio2->CreateMasteringVoice(&pMasterVoice);
}

void AudioManager::Play(
    Resource<ResourceType::Audio>& _resource,
    float _volume)
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

	// 音量を設定
	pSourceVoice->SetVolume(std::clamp(_volume, 0.0f, 1.0f));

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

// 2つの効果音を順番に再生
void AudioManager::PlaySequence(
    Resource<ResourceType::Audio>& _first,
    Resource<ResourceType::Audio>& _next)
{
	AudioResource& audio = _first.Get();

	if (!pXAudio2)
	{
		return;
	}

	// 1つ目の音声を再生
	Play(_first);

	// 1つ目の音声の再生時間を取得
	if (audio.waveFormatEx.Format.nAvgBytesPerSec == 0)
	{
		return;
	}

	sequenceTimeLeft = static_cast<float>(audio.buffer.size()) / audio.waveFormatEx.Format.nAvgBytesPerSec;

	// 次に再生する音声を登録
	pNextSequenceAudio = &_next;
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

// 蘇生開始SEを再生
void AudioManager::PlayResuscitation(
    Resource<ResourceType::Audio>& _resource)
{
	AudioResource& audio = _resource.Get();

	if (!pXAudio2)
	{
		return;
	}

	// すでに再生中の蘇生SEがあれば停止
	if (pResuscitationSourceVoice)
	{
		pResuscitationSourceVoice->Stop();
		pResuscitationSourceVoice->DestroyVoice();
		pResuscitationSourceVoice = nullptr;
	}

	isResuscitationFading    = false;
	resuscitationFadeElapsed = 0.0f;

	HRESULT hResult = pXAudio2->CreateSourceVoice(&pResuscitationSourceVoice, &audio.waveFormatEx.Format);

	if (FAILED(hResult) || pResuscitationSourceVoice == nullptr)
	{
		return;
	}

	// 通常の音量で再生
	pResuscitationSourceVoice->SetVolume(1.0f);

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

	hResult = pResuscitationSourceVoice->SubmitSourceBuffer(&BUFFER);

	if (FAILED(hResult))
	{
		pResuscitationSourceVoice->DestroyVoice();
		pResuscitationSourceVoice = nullptr;
		return;
	}

	hResult = pResuscitationSourceVoice->Start();

	if (FAILED(hResult))
	{
		pResuscitationSourceVoice->DestroyVoice();
		pResuscitationSourceVoice = nullptr;
		return;
	}
}

// 蘇生開始SEをフェードアウト
void AudioManager::FadeOutResuscitation()
{
	if (!pResuscitationSourceVoice)
	{
		return;
	}

	// すでに自然に再生終了している場合は何もしない
	XAUDIO2_VOICE_STATE state{};
	pResuscitationSourceVoice->GetState(&state);

	if (state.BuffersQueued == 0)
	{
		pResuscitationSourceVoice->DestroyVoice();
		pResuscitationSourceVoice = nullptr;
		isResuscitationFading     = false;
		resuscitationFadeElapsed  = 0.0f;
		return;
	}

	// フェードアウト開始
	isResuscitationFading    = true;
	resuscitationFadeElapsed = 0.0f;
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

// 着地音を再生
void AudioManager::PlayLanding(
    Resource<ResourceType::Audio>& _resource)
{
	AudioResource& audio = _resource.Get();

	if (!pXAudio2)
	{
		return;
	}

	// 初回だけSourceVoiceを作成
	if (!pLandingSourceVoice)
	{
		HRESULT hResult = pXAudio2->CreateSourceVoice(&pLandingSourceVoice, &audio.waveFormatEx.Format);

		if (FAILED(hResult) || pLandingSourceVoice == nullptr)
		{
			return;
		}
	}

	// 再生中なら停止してバッファをクリア
	pLandingSourceVoice->Stop();
	pLandingSourceVoice->FlushSourceBuffers();

	// 着地音の音量を1.5倍に設定
	pLandingSourceVoice->SetVolume(1.5f);

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

	HRESULT hResult = pLandingSourceVoice->SubmitSourceBuffer(&BUFFER);

	if (FAILED(hResult))
	{
		return;
	}

	hResult = pLandingSourceVoice->Start();

	if (FAILED(hResult))
	{
		return;
	}
}

// 弓を引き絞る音をループ再生
void AudioManager::PlayBowPull(
    Resource<ResourceType::Audio>& _resource)
{
	AudioResource& audio = _resource.Get();

	if (!pXAudio2)
	{
		return;
	}

	// 初回だけSourceVoiceを作成
	if (!pBowPullSourceVoice)
	{
		HRESULT hResult = pXAudio2->CreateSourceVoice(&pBowPullSourceVoice, &audio.waveFormatEx.Format);

		if (FAILED(hResult) || pBowPullSourceVoice == nullptr)
		{
			return;
		}
	}

	// 再生中なら停止してバッファをクリア
	pBowPullSourceVoice->Stop();
	pBowPullSourceVoice->FlushSourceBuffers();

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

	HRESULT hResult = pBowPullSourceVoice->SubmitSourceBuffer(&BUFFER);

	if (FAILED(hResult))
	{
		return;
	}

	hResult = pBowPullSourceVoice->Start();

	if (FAILED(hResult))
	{
		return;
	}
}

// 弓を引き絞る音を停止
void AudioManager::StopBowPull()
{
	if (pBowPullSourceVoice)
	{
		pBowPullSourceVoice->Stop();
		pBowPullSourceVoice->FlushSourceBuffers();
	}
}

// 着地音を停止
void AudioManager::StopLanding()
{
	if (pLandingSourceVoice)
	{
		pLandingSourceVoice->Stop();
		pLandingSourceVoice->FlushSourceBuffers();
	}
}

// BGMを再生
void AudioManager::SetBGM(
    Resource<ResourceType::Audio>& _resource,
    bool _isLoop,
    float _volume)
{
	AudioResource& audio = _resource.Get();

	if (!pXAudio2)
	{
		return;
	}

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

	// 音量を設定
	pBGMSourceVoice->SetVolume(std::clamp(_volume, 0.0f, 1.0f));

	const XAUDIO2_BUFFER BUFFER{
		.Flags      = XAUDIO2_END_OF_STREAM,
		.AudioBytes = static_cast<UINT32>(audio.buffer.size()),
		.pAudioData = audio.buffer.data(),
		.PlayBegin  = 0,
		.PlayLength = 0,
		.LoopBegin  = 0,
		.LoopLength = 0,
		.LoopCount  = static_cast<UINT32>(_isLoop ? XAUDIO2_LOOP_INFINITE : 0),
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

// BGMが再生中か確認
bool AudioManager::IsBGMPlaying() const
{
	if (!pBGMSourceVoice)
	{
		return false;
	}

	XAUDIO2_VOICE_STATE state{};
	pBGMSourceVoice->GetState(&state);

	return state.BuffersQueued > 0;
}

void AudioManager::LateUpdate(
    const float _deltaTime)
{
	// 連続再生の管理
	if (pNextSequenceAudio)
	{
		sequenceTimeLeft -= _deltaTime;

		// 1つ目の音声が終了したら2つ目を再生
		if (sequenceTimeLeft <= 0.0f)
		{
			Resource<ResourceType::Audio>* pNext = pNextSequenceAudio;

			// 再生前に解除
			pNextSequenceAudio = nullptr;
			sequenceTimeLeft   = 0.0f;

			Play(*pNext);
		}
	}

	// ここから下は既存の処理
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

	// 蘇生SEのフェードアウト管理
	if (pResuscitationSourceVoice)
	{
		XAUDIO2_VOICE_STATE state{};
		pResuscitationSourceVoice->GetState(&state);

		// 自然に再生が終了した場合
		if (state.BuffersQueued == 0)
		{
			pResuscitationSourceVoice->DestroyVoice();
			pResuscitationSourceVoice = nullptr;

			isResuscitationFading    = false;
			resuscitationFadeElapsed = 0.0f;
		}
		// ボタンを離してフェードアウト中の場合
		else if (isResuscitationFading)
		{
			resuscitationFadeElapsed += _deltaTime;

			float rate = 1.0f - (resuscitationFadeElapsed / RESUSCITATION_FADE_TIME);

			rate = std::clamp(rate, 0.0f, 1.0f);

			pResuscitationSourceVoice->SetVolume(rate);

			// 0.5秒経過したら停止・破棄
			if (resuscitationFadeElapsed >= RESUSCITATION_FADE_TIME)
			{
				pResuscitationSourceVoice->Stop();
				pResuscitationSourceVoice->DestroyVoice();
				pResuscitationSourceVoice = nullptr;

				isResuscitationFading    = false;
				resuscitationFadeElapsed = 0.0f;
			}
		}
	}
}

void AudioManager::Update() {}

// BGMを停止・再生する処理は上記の通り

void AudioManager::Release()
{
	// 連続再生の情報をリセット
	pNextSequenceAudio = nullptr;
	sequenceTimeLeft   = 0.0f;

	// 足音を停止
	StopFootstep();

	// ジャンプ音を停止・破棄
	if (pJumpSourceVoice)
	{
		pJumpSourceVoice->Stop();
		pJumpSourceVoice->DestroyVoice();
		pJumpSourceVoice = nullptr;
	}

	// 弓を引き絞る音を停止・破棄
	if (pBowPullSourceVoice)
	{
		pBowPullSourceVoice->Stop();
		pBowPullSourceVoice->DestroyVoice();
		pBowPullSourceVoice = nullptr;
	}

	// 着地音を停止・破棄
	if (pLandingSourceVoice)
	{
		pLandingSourceVoice->Stop();
		pLandingSourceVoice->DestroyVoice();
		pLandingSourceVoice = nullptr;
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

	// 蘇生SEを停止・破棄
	if (pResuscitationSourceVoice)
	{
		pResuscitationSourceVoice->Stop();
		pResuscitationSourceVoice->DestroyVoice();
		pResuscitationSourceVoice = nullptr;
	}

	isResuscitationFading    = false;
	resuscitationFadeElapsed = 0.0f;

	players.clear();
}
