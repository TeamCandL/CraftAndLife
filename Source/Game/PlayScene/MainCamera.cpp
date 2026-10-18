#include "pch\pch.h"
#include "MainCamera.h"
#include "MainCamera/IMainCameraUpdater.h"
#include "MainCamera/GhostCamera.h"
#include "MainCamera/TPSCamera.h"

void MainCamera::Start()
{
	objUpdater = GetScene().Instantiate<GhostCamera>();
	position = { 0.0f, 0.0f, -10.0f };
}

void MainCamera::Update()
{
	RawMouseInputManager& rawMouseInput = Get<cha::RawMouseInputManager>();
	CursorSystem& cursor = Get<CursorSystem>();
	InputSystem& input = Get<InputSystem>();
	CameraSystemData& camera = Get<CameraSystem>().Data();

	ImGui::Begin("DebugCamera");

	ImGui::DragFloat3("position", camera.transform.position.v);

	if (ImGui::Button("Ghost"))
	{
		GetScene().Destroy(objUpdater);
		objUpdater = GetScene().Instantiate<GhostCamera>();
	}
	if (ImGui::Button("TPS"))
	{
		GetScene().Destroy(objUpdater);
		objUpdater = GetScene().Instantiate<TPSCamera>();
	}

	ImGui::End();

	//#ifdef _DEBUG
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse || io.WantCaptureKeyboard)
	{
		// ImGui操作中なら無視
		return;
	}
	//#endif


	if (rawMouseInput.GetMouseDown(MouseCode_Left))
	{
		isHideCursor = true;
		cursor.SetVisible(false);  // 非表示に
		cursor.ChangeMode(CursorMode_CenterLock);
	}

	//if (!isHideCursor)
	//{
	//	return;  // カーソルが非表示なら操作しない
	//}

	if (input.GetKeyDown(KeyCode::Escape))
	{
		isHideCursor = false;
		cursor.SetVisible(true);  // 表示する
		cursor.ChangeMode(CursorMode_Free);
		return;
	}

	//if (cursor.IsVisible())
	//{
	//	return;  // カーソルが表示されているなら操作しない
	//}

	if (objUpdater)
	{
		Get<IMainCameraUpdater>(objUpdater).Update(Data());
	}
}

void MainCamera::SetPosition(const Vec3 _worldPosition)
{
	position = _worldPosition;

	CameraSystemData& cameraSystemData = Get<CameraSystem>().Data();
	cameraSystemData.transform.position = position;
}

void MainCamera::ChangeUpdater(const GameObject _objUpdater)
{
	VCU_ASSERT(_objUpdater.IsEmpty() && "渡された更新処理オブジェクトが空っぽであった");
	
	GetScene().Destroy(objUpdater);
	objUpdater = _objUpdater;
}

void MainCamera::UpdatePositionFromTarget(const GameObject _objTarget)
{
	if (!objUpdater.IsEmpty())
	{
		Get<IMainCameraUpdater>(objUpdater).UpdatePositionFromTarget(Data(), _objTarget);
	}
}

float MainCamera::GetAngleX() const
{
	return angles.x;
}

float MainCamera::GetAngleY() const
{
	return std::fmodf(angles.y, DirectX::XM_2PI);
}
