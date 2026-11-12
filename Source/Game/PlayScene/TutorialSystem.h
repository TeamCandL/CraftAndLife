#pragma once
#include <Game/Game.h>
#include "TutorialPhaseType.h"

/// @brief チュートリアルを管理するシステムのデータ
struct TutorialSystemData
{
	GameObject objObserver{};  ///< チュートリアルオブザーバーオブジェクト

	TutorialElementFlags elementClearFlags{};  ///< 各要素をクリアしたかのフラグ
};

/// @brief チュートリアルを管理するシステム
struct TutorialSystem : ObjectBase<TutorialSystemData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;

	/// @brief 強制的にチュートリアルオブザーバーオブジェクトを変更する
	/// @param _obj オブザーバーオブジェクト
	void ChangeObserverForce(const GameObject _obj);
};