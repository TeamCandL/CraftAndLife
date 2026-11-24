#include "pch\pch.h"
#include "TutorialFirstStep.h"
#include <Game/PlayScene/TutorialSystem.h>
#include <Game/System/AbstructInputSystem.h>
#include <Game/System/AbstructInputSystem/TutorialPlayerInput.h>
#include <Game/System/AbstructInputSystem/TutorialUIInput.h>
#include <Game/PlayScene/MainCamera.h>
#include <Game/PlayScene/Player.h>
#include <Game/PlayScene/PlayerHand.h>
#include <Game/PlayScene/PlaySceneUIController.h>
#include <Game/PlayScene/UI/TaskBackground.h>
#include <Game/PlayScene/UI/TutorialUI.h>

void TutorialFirstStep::Start()
{
	TutorialSystem& tutorialSystem           = Get<TutorialSystem>();
	AbstructInputSystem& abstructInputSystem = Get<AbstructInputSystem>();

	tutorialSystem.ChangeObserverForce(this->ToGameObject());

	previousPlayerPosition = Get<Transform>(objPlayer).position;
	previousCameraAngles   = Get<MainCamera>(objMainCamera).GetAngle();

	objTaskBackground = Get<PlaySceneUIController>(objPlaySceneUIController).Data().objTaskBackground;
	objTutorialUI     = Get<PlaySceneUIController>(objPlaySceneUIController).Data().objTutorialUI;
	Get<TaskBackground>(objTaskBackground).SetTask("右(→)スティックで\n周りを見よう！");
	Get<TutorialSystem>().SetFocusType(TutorialForcusType_MoveCamera);
	Get<TutorialUI>(objTutorialUI).SetImage(TutorialUIData::TutorialImageType_Camera);

	objPlayerHand = Get<Player>(objPlayer).Data().objPlayerHand;

	SetTutorialInputEnable(true);
}

void TutorialFirstStep::FillterCheckCleardElement(
    TutorialElementFlags& _flags,
    const TutorialFocusType _type)
{
	// フラグが降ろされている要素をチェックするため、指定の要素のみ確かめたい場合は 他全クリア判定=最初all set してね
	switch (_type)
	{
	case TutorialForcusType_MoveBody:
		_flags.set();
		_flags.reset(TutorialElementType_CheckBodyMove);
		break;
	case TutorialForcusType_MoveCamera:
		_flags.set();
		_flags.reset(TutorialElementType_CheckCameraMove);
		break;
	case TutorialForcusType_JumpBody:
		_flags.set();
		_flags.reset(TutorialElementType_CheckJumpMove);
		break;
	case TutorialForcusType_SelectBackpack:
		_flags.set();
		_flags.reset(TutorialElementType_CheckSelectBackpack);
		break;
	case TutorialForcusType_OpenBackpack:
		_flags.set();
		_flags.reset(TutorialElementType_CheckOpenBackpack);
		break;
	case TutorialForcusType_CloseBackpack:
		_flags.set();
		_flags.reset(TutorialElementType_CheckCloseBackpack);
		break;
	case TutorialForcusType_None_Max:
	default:
		break;
	}
}

bool TutorialFirstStep::CheckClearedTutorial(
    const TutorialElementType _type)
{
	switch (_type)
	{
	case TutorialElementType_CheckCameraMove:
		totalCameraAngles += previousCameraAngles - Get<MainCamera>(objMainCamera).GetAngle();
		previousCameraAngles = Get<MainCamera>(objMainCamera).GetAngle();

		return (totalCameraAngles).Length() >= COMPLITE_CAMERA_MOVE_ANGLES_THRESHOLD;

	case TutorialElementType_CheckBodyMove:
		totalPlayerMove += previousPlayerPosition - Get<Transform>(objPlayer).position;
		previousPlayerPosition = Get<Transform>(objPlayer).position;

		return (totalPlayerMove).Length() >= COMPLITE_BODY_MOVE_DISTANCE_THRESHOLD;

	case TutorialElementType_CheckJumpMove:
		return Get<Transform>(objPlayer).position.y - previousPlayerPosition.y >= 1.0f;
	case TutorialElementType_CheckSelectBackpack:
		return Get<PlayerHand>(objPlayerHand).Data().haveItemId == ITEM_ID_BACKPACK;
	case TutorialElementType_CheckOpenBackpack:
		return Get<Player>(objPlayer).Data().isOpenCraftUI;
	case TutorialElementType_CheckCloseBackpack:
		return !(Get<Player>(objPlayer).Data().isOpenCraftUI);
	default:
		break;
	}

	return false;
}

GameObject TutorialFirstStep::GetNextObserverOnCleardElement(
    const TutorialElementType _cleardElement,
    const TutorialElementFlags& _allFlags)
{
	if (_cleardElement == TutorialElementType_CheckCameraMove)
	{
		Get<TaskBackground>(objTaskBackground).SetTask("右(→)スティックで\n動き回ろう！");
		Get<TutorialSystem>().SetFocusType(TutorialForcusType_MoveBody);
		Get<TutorialUI>(objTutorialUI).SetImage(TutorialUIData::TutorialImageType_Body);
	}
	else if (_cleardElement == TutorialElementType_CheckBodyMove)
	{
		Get<TaskBackground>(objTaskBackground).SetTask("Aボタンで\n飛び回ろう！");
		Get<TutorialSystem>().SetFocusType(TutorialForcusType_JumpBody);
		Get<TutorialUI>(objTutorialUI).SetImage(TutorialUIData::TutorialImageType_Jump);
	}
	else if (_cleardElement == TutorialElementType_CheckJumpMove)
	{
		Get<TaskBackground>(objTaskBackground).SetTask("トリガー上の左右ボタンで\nバックパックを選ぼう！");
		Get<TutorialSystem>().SetFocusType(TutorialForcusType_SelectBackpack);
		Get<TutorialUI>(objTutorialUI).SetImage(TutorialUIData::TutorialImageType_Hotbar);
	}
	else if (_cleardElement == TutorialElementType_CheckSelectBackpack)
	{
		Get<TaskBackground>(objTaskBackground).SetTask("右トリガーボタンで\nバックパックを開いてみよう！");
		Get<TutorialSystem>().SetFocusType(TutorialElementType_CheckOpenBackpack);
		Get<TutorialUI>(objTutorialUI).SetImage(TutorialUIData::TutorialImageType_OpenBackpack);
	}
	else if (_cleardElement == TutorialElementType_CheckOpenBackpack)
	{
		Get<TaskBackground>(objTaskBackground).SetTask("もう一度右トリガーボタンで\nバックパックを閉じよう。");
		Get<TutorialSystem>().SetFocusType(TutorialElementType_CheckCloseBackpack);
		Get<TutorialUI>(objTutorialUI).SetImage(TutorialUIData::TutorialImageType_CloseBackpack);
	}
	else if (_cleardElement == TutorialElementType_CheckCloseBackpack)
	{
		Get<TaskBackground>(objTaskBackground).SetEnableDayTimer(true);
		Get<TutorialSystem>().SetFocusType(TutorialForcusType_None_Max);
		SetTutorialInputEnable(false);
		Get<TutorialUI>(objTutorialUI).SetImage(TutorialUIData::TutorialImageType_Max);
	}

	return GameObject::Empty();
}

void TutorialFirstStep::SetTutorialInputEnable(
    const bool _enabled)
{
	if (_enabled)
	{
		TutorialSystem& tutorialSystem           = Get<TutorialSystem>();
		AbstructInputSystem& abstructInputSystem = Get<AbstructInputSystem>();

		Get<AbstructInputSystem>().SetPlayerInput(
		    std::make_shared<TutorialPlayerInput>(tutorialSystem, abstructInputSystem));
		Get<AbstructInputSystem>().SetUIInput(std::make_shared<TutorialUIInput>(tutorialSystem, abstructInputSystem));
	}
	else
	{
		Get<AbstructInputSystem>().ResetInput();
	}
}
