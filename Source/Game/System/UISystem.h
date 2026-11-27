#pragma once
#include <Game/Game.h>
#include <Game/Common/UICircleImageMaterial.h>

/// @brief UIのシステムのデータ
struct UISystemData
{
	Resource<ResourceType::Shader> uiShader{
		"Shader/UI.hlsl",
		RasterizerConfig{ .cullMode = D3D11_CULL_NONE, .fillMode = D3D11_FILL_SOLID, .frontCounterClockwise = false }
	};  // UI用のシェーダ

	Resource<ResourceType::Shader> uiImageCircleShader{
		"Shader/UIImageCircle.hlsl",
		RasterizerConfig{ .cullMode = D3D11_CULL_NONE, .fillMode = D3D11_FILL_SOLID, .frontCounterClockwise = false }
	};  // UI円画像用のシェーダ

	Resource<ResourceType::Surface> surfaceQuad{};  // 四角の面
	Resource<ResourceType::Material> material{ MaterialResource::InstanceEmpty<UITransformMaterialProperty>() };
	Resource<ResourceType::Material> materialImageCircle{
		MaterialResource::InstanceEmpty<UICircleImageMaterialProperty>()
	};

	bool isWantCaptureMouse{ false };  // マウス操作をUIが奪ったか
	bool isHideHUD{ false };           ///< HUD非表示か true / false
};

/// @brief UIのシステム
struct UISystem : SystemObjectBase<UISystem, UISystemData>
{
	/// @brief 開始処理
	void Start() override;

	/// @brief 毎フレームの更新処理
	void Update() override;

	/// @brief マウスがUI上で操作されたか
	/// @return UI上の操作があった true / false
	inline bool WantCaptureMouse() const
	{
		return isWantCaptureMouse;
	}

	/// @brief HUDを非表示にするかを設定
	/// @param _isHideHUD HUD非表示 true / false
	void SetHideHUD(
	    bool _isHideHUD)
	{
		isHideHUD = _isHideHUD;
	}
	/// @brief HUDが非表示であるか
	/// @return 非表示である true / false
	bool IsHideHUD() const
	{
		return isHideHUD;
	}

  private:
	/// @brief ワールド座標と紐づける処理の更新
	void UpdateBindWorld();

	/// @brief 描画するコンポーネントの更新
	void UpdateRenderer();

	/// @brief テキストの描画更新
	void UpdateText();

	/// @brief レイキャストの更新
	void UpdateRaycast();
};
