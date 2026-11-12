#pragma once
#include <Game/Game.h>
#include "TutorialPhaseType.h"

/// @brief チュートリアルを管理するシステムのデータ
struct TutorialSystemData
{
	GameObject objObserver{};  ///< チュートリアルオブザーバーオブジェクト

	TutorialPhaseFlags phaseClearFlags{};  ///< 各フェーズをクリアしたかのフラグ
};

/// @brief チュートリアルを管理するシステム
struct TutorialSystem : ObjectBase<TutorialSystemData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};