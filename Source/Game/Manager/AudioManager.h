
#pragma once
#include <Base/Base.h>
#include "AudioManager/AudioPlayer.h"
#include <vector>

struct AudioManager : IManager
{
	/// @brief 開始処理
	void Start();

	/// @brief 更新処理
	void Update();

	/// @brief BGMを設定する
	/// @param _resource 音声リソース
	/// @param _isLoop ループ再生するか
	/// @param _volume 音量（0.0～1.0）
	void SetBGM(Resource<ResourceType::Audio>& _resource, bool _isLoop = true, float _volume = 1.0f);

	/// @brief 再生終了時にループを離脱する
	void SetBGMExitLoop();

	/// @brief BGMを止める
	void SetBGMStop();

	/// @brief BGMを再開する
	void SetBGMStart();

	// 環境音を再生
	void SetAmbientSound(Resource<ResourceType::Audio>& _resource);

	// 環境音を停止
	void StopAmbientSound();

	/// @brief 効果音を再生する
	/// @param _resource 音声リソース
	void Play(Resource<ResourceType::Audio>& _resource);

	// 2つの効果音を順番に再生する
	void PlaySequence(Resource<ResourceType::Audio>& _first, Resource<ResourceType::Audio>& _next);

	/// @brief 毎フレーム呼ばれる更新処理
	void LateUpdate(const float _deltaTime);

	/// @brief 廃棄処理
	void Release() override;

	// 足音を再生する
	void PlayFootstep(Resource<ResourceType::Audio>& _resource);

	// 足音を停止する
	void StopFootstep();

	// ジャンプ音を再生する
	void PlayJump(Resource<ResourceType::Audio>& _resource);

	// ジャンプ音を停止する
	void StopJump();

  private:
	ComPtr<IXAudio2> pXAudio2{};
	IXAudio2MasteringVoice* pMasterVoice{ nullptr };

	IXAudio2SourceVoice* pBGMSourceVoice{ nullptr };

	IXAudio2SourceVoice* pAmbientSourceVoice{ nullptr };

	IXAudio2SourceVoice* pFootstepSourceVoice{ nullptr };

	IXAudio2SourceVoice* pJumpSourceVoice{ nullptr };

	std::vector<AudioPlayer> players{};

	// 連続再生用
	float sequenceTimeLeft{ 0.0f };
	Resource<ResourceType::Audio>* pNextSequenceAudio{ nullptr };
};
