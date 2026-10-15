# コーディングガイドライン

Player.h

```cpp
#pragma once
#include <Game/Game.h>
#include "IPlayerController.h"

/// @brief プレイヤーのデータ
struct PlayerData
{
	/// @brief コンストラクタ
	/// @param _objCamera カメラのオブジェクト
	PlayerData(const GameObject _objCamera) :
		objCamera{ _objCamera }
	{}

	Component<Transform> transform{};                        ///< 座標系
	int32_t hp{ 32 };                                        ///< 体力値
	GameObject objCamera{};                                  ///< カメラのオブジェクト
	std::unique_ptr<IPlayerController> pPlayerController{};  ///< プレイヤー操作
};

/// @brief プレイヤー
struct Player : ObjectBase<PlayerData>
{
	using ObjectBase<PlayerData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;

	/// @brief 更新処理
	void Update() override;
};
```

Player.cpp

```cpp
#include "Player.h"

void Player::Start()
{
	
}

void Player::Update()
{
	const float DELTA_TIME = Get<DeltaTime>();

	transform.position.x += DELTA_TIME * 100.0f;
}
```

IPlayerController.h

```cpp
#include <Game/Game.h>

/// @brief プレイヤー操作のインタフェース
class IPlayerController
{
public:
	/// @brief コンストラクタ
	virtual IPlayerController() = deafult;
	/// @brief デストラクタ
	virtual ~IPlayerController() = deafult;

	/// @brief 更新処理
	virtual void Update(PlayerData& _playerData) = 0;
};
```

NormalPlayerController.h

```cpp
#include <Game/Game.h>

/// @brief プレイヤー普通の操作
class NormalPlayerController : public IPlayerController
{
public:
	/// @brief コンストラクタ
	/// @param _moveSpeedPerSec プレイヤーの移動速度(/sec)
	NormalPlayerController(const float _moveSpeedPerSec);
	/// @brief デストラクタ
	~NormalPlayerController() = default;

	/// @brief 更新処理
	void Update(PlayerData& _playerData) override;

	/// @brief 移動速度を取得する
	/// @return 移動速度
	float GetMoveSpeedPerSec() const { return moveSpeedPerSec_; }
	/// @brief 移動速度を設定する
	/// @param _moveSpeedPerSec プレイヤーの移動速度(/sec)
	void SetMoveSpeedPerSec(const float _moveSpeedPerSec) const { moveSpeedPerSec_ = _moveSpeedPerSec; }

private:
	float moveSpeedPerSec_{};  ///< 1秒間あたりの移動スピード
};
```
