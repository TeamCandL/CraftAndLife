#pragma once
#include <Game/Game.h>
#include "TutorialPhaseType.h"

/// @brief チュートリアルを管理するシステムのデータ
struct TutorialSystemData
{
	GameObject objObserver{};  ///< チュートリアルオブザーバーオブジェクト

	TutorialElementFlags elementClearFlags{};  ///< 各要素をクリアしたかのフラグ

	TutorialFocusType focusType{};  ///< 現在の集中タイプ
};

/// @brief チュートリアルを管理するシステム
struct TutorialSystem : SystemObjectBase<TutorialSystem, TutorialSystemData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;

	/// @brief 強制的にチュートリアルオブザーバーオブジェクトを変更する
	/// @param _obj オブザーバーオブジェクト
	void ChangeObserverForce(const GameObject _obj);

	/// @brief 現在の集中タイプを取得する
	/// @return 現在の集中タイプ
	TutorialFocusType GetFocusType() const
	{
		return focusType;
	}

	/// @brief 現在の集中タイプを設定する
	/// @param _type 設定する集中タイプ
	void SetFocusType(
	    const TutorialFocusType _type = TutorialForcusType_None_Max)
	{
		focusType = _type;
	}
};