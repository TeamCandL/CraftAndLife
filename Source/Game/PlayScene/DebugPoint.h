#pragma once
#include <Game/Game.h>

/// @brief デバッグ用の球体のデータ
struct DebugPointData
{
	DebugPointData() {}
	DebugPointData(
	    const Vec3 _position)
	{
		transform.position = _position;
	}
	Resource<Shader> shader{ "Shader/Unlit.hlsl" };                        ///< シェーダ
	Resource<Model> model{ "Model/SimpleSphere/SimpleSphere.fbx" };        ///< モデル
	Component<Transform> transform{ Vec3::Zero(), Vec3::One() * 100.0f };  ///< 座標系
};

/// @brief デバッグ用の球体
struct DebugPoint : ObjectBase<DebugPointData>
{
	using ObjectBase<DebugPointData>::ObjectBase;
	/// @brief 描画処理
	void Draw() override;
	/// @brief 削除されたよおお
	void OnDestroy() override;
};
