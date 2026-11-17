#include "pch\pch.h"
#include "TutorialSystem.h"
#include "Tutorial/ITutorialObserver.h"

void TutorialSystem::Start() {}

void TutorialSystem::Update()
{
	if (objObserver.IsEmpty())
	{
		return;  // ターゲットのオブザーバーがないなら無視
	}

	TutorialElementFlags flags  = elementClearFlags;
	ITutorialObserver& observer = Get<ITutorialObserver>(objObserver);

	// フォーカスタイプに応じてチェックするフラグをフィルタリングする
	observer.FillterCheckCleardElement(flags, focusType);

	for (TutorialElementType type{}; type < TutorialElementType_Max; type++)
	{
		if (flags[type] || elementClearFlags[type])
		{
			continue;  // 既にクリアしているなら無視！
		}

		// 指定の要素がクリアされたか更新する
		elementClearFlags[type] = observer.CheckClearedTutorial(type);
		if (elementClearFlags[type])
		{
			// 指定の要素をクリアしたとき、チェンジするオブザーバーがあるか取得する
			GameObject objNextObserver = observer.GetNextObserverOnCleardElement(type, elementClearFlags);
			if (!objNextObserver.IsEmpty())
			{
				// チェンジするオブザーバーがあるなら設定！
				objObserver = objNextObserver;
				return;  // オブザーバー設定したから今回はここで終わり
			}
		}
	}
}

void TutorialSystem::ChangeObserverForce(
    const GameObject _obj)
{
	objObserver = _obj;
}
