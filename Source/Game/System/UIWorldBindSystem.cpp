#include "pch\pch.h"
#include "UIWorldBindSystem.h"
#include <Game/Component/UI/UIWorldBinder.h>
#include <Game/Component/UI/UITransform.h>
#include <Game/Component/Transform.h>

void UIWorldBindSystem::LateUpdate()
{
	auto view = GetView<UITransform, Transform, UIWorldBinder>();

	// スクリーンのサイズ
	const Vec2 SCREEN_SIZE              = Get<ScreenSize>();
	const CameraSystemData& CAMERA_DATA = Get<CameraSystem>().Data();

	view.ForEach(
	    [SCREEN_SIZE, &CAMERA_DATA](
	        IObject& _object,
	        UITransform& _uiTransform,
	        Transform& _transform,
	        UIWorldBinder& _uiWorldBinder)
	    {
		    if (_uiWorldBinder.baseScale.x * _uiWorldBinder.baseScale.y <= FLT_EPSILON)
		    {
			    // xy どちらかが0ならスケールを保存しとく
			    _uiWorldBinder.baseScale    = _uiTransform.scale;
			    _uiWorldBinder.basePosition = _uiTransform.position;
		    }
		    // ワールド座標
		    const Vec3 WORLD_POSITION = _transform.WorldPosition();

		    // 3D座標を2D座標に変換 (変換後のスクリーン座標)
		    const Vec3 SCREEN_POSITION = DirectX::XMVector3Project(
		        WORLD_POSITION,
		        0.0f,
		        0.0f,
		        SCREEN_SIZE.x,
		        SCREEN_SIZE.y,
		        0.0f,
		        1.0f,
		        CAMERA_DATA.projectionMatrix,
		        CAMERA_DATA.viewMatrix,
		        DirectX::XMMatrixIdentity());

		    // カメラの後ろにいる場合は表示しない
		    if (SCREEN_POSITION.z < 0.0f || 1.0f < SCREEN_POSITION.z)
		    {
			    _uiTransform.scale = Vec2::Zero();
			    return;
		    }

		    // UIサイズを元に戻す
		    _uiTransform.scale = _uiWorldBinder.baseScale;

		    // UIの位置を確定
		    _uiTransform.position = _uiWorldBinder.basePosition + Vec2{ SCREEN_POSITION };
	    });
}
