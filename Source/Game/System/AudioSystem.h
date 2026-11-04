#pragma once
#include <Base/Base.h>
#include "AudioSystem/AudioPlayer.h"
#include <vector>

struct AudioSystem : SystemObjectBase<AudioSystem>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
	/// @brief BGMを設定する
	/// @param _resource 音声リソース
	void SetBGM(Resource<ResourceType::Audio>& _resource);
	/// @brief 再生終了時にループを離脱する
	void SetBGMExitLoop();
	/// @brief BGMを止める
	void SetBGMStop();
	/// @brief BGMを再開する
	void SetBGMStart();
	/// @brief 効果音を再生する
	/// @param _resource 音声リソース
	void Play(Resource<ResourceType::Audio>& _resource);

	/// @brief 毎フレーム呼ばれる更新処理
	void LateUpdate() override;
	/// @brief 廃棄処理
	void OnDestroy() override;

	// 足音を再生する
	void PlayFootstep(Resource<ResourceType::Audio>& _resource);

	// 足音を停止する
	void StopFootstep();

  private:
	ComPtr<IXAudio2> pXAudio2{};
	IXAudio2MasteringVoice* pMasterVoice{ nullptr };
	IXAudio2SourceVoice* pBGMSourceVoice{ nullptr };  // BGM用の音声ソース

	IXAudio2SourceVoice* pFootstepSourceVoice{ nullptr };

	std::vector<AudioPlayer> players{};  // 再生中の音声プレイヤー
};
