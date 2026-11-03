#include "pch\pch.h"
#include "AnimationSystem.h"
#include "../Component/Transform.h"
#include "../Component/TransformAnimator.h"

void AnimationSystem::Update()
{
	auto view = GetView<Transform, TransformAnimator>();

	const float DELTA_TIME = Get<DeltaTime>();

	view.ForEach(
	    [DELTA_TIME](IObject& _object, Transform& _transform, TransformAnimator& _animator)
	    {
		    if (_animator.playAnimation.size() <= 0)
		    {
			    return;  // 再生中のアニメーションが空っぽなら無視
		    }

		    _animator.onFinished = false;
		    while (true)
		    {
			    auto itr = _animator.animations.find(_animator.playAnimation);

			    if (itr == _animator.animations.end())
			    {
				    VCU_ASSERT(false && "存在しないアニメーション");
				    return;  // 再生中のアニメーションが無いなら無視
			    }

			    auto& [name, animKeys] = *itr;

			    _animator.playingTimeSec += DELTA_TIME;

			    auto itrEnd = std::find_if(
			        animKeys.begin(),
			        animKeys.end(),
			        [&_animator](const TransformAnimationKey& _key)
			        {
				        return _animator.playingTimeSec < _key.timeAtSec;
			        });

			    if (itrEnd == animKeys.begin())
			    {
				    // これ以上後ろがないため開始キーに合わせる
				    _transform.position  = itrEnd->position;
				    _transform.rotation  = itrEnd->rotation;
				    _transform.scale     = itrEnd->scale;
				    _animator.isFinished = false;
				    return;
			    }

			    if (itrEnd == animKeys.end())
			    {
				    // 番兵さんに当たっている
				    _animator.isFinished = true;  // 次ループに入る入らない関係なく終了フラグは立てる。

				    if (_animator.playToLoop)
				    {
					    _animator.playToLoop = false;  // ループしたためフラグを降ろしておく
					    // ループ用に時間を調整して戻す
					    _animator.playingTimeSec = std::fmodf(_animator.playingTimeSec, animKeys.back().timeAtSec);
					    _animator.onFinished     = true;
					    continue;
				    }
				    else
				    {
					    _transform.position      = animKeys.back().position;
					    _transform.rotation      = animKeys.back().rotation;
					    _transform.scale         = animKeys.back().scale;
					    _animator.playingTimeSec = animKeys.back().timeAtSec;
					    return;
				    }
			    }
			    else
			    {
				    _animator.isFinished = false;
			    }

			    auto itrBegin = itrEnd - 1;

			    const float RATIO =
			        (_animator.playingTimeSec - itrBegin->timeAtSec) / (itrEnd->timeAtSec - itrBegin->timeAtSec);

			    _transform.position = Mathf::Lerp(itrBegin->position, itrEnd->position, RATIO);
			    _transform.rotation = DirectX::XMQuaternionSlerp(itrBegin->rotation, itrEnd->rotation, RATIO);
			    _transform.scale    = Mathf::Lerp(itrBegin->scale, itrEnd->scale, RATIO);

			    return;
		    }
	    });
}
