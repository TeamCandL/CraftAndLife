#pragma once
#include "IPlayerInput.h"
#include "../AbstructInputSystem.h"

/// @brief 普通のプレイヤー入力
class NormalPlayerInput : public IPlayerInput
{
public:
	/// @brief コンストラクタ
	/// @param _abstructInputSystem 抽象入力システムの参照ポインタ
	NormalPlayerInput(AbstructInputSystem& _abstructInputSystem);
	/// @brief デストラクタ
	~NormalPlayerInput() = default;

	// IPlayerInput を介して継承されました
	Vec2 GetMoveBody() const override;
	Vec2 GetMoveCamera() const override;
	bool IsActionBegin() const override;
	bool IsAction() const override;
	bool IsActionEnd() const override;
	bool IsSubActionBegin() const override;
	bool IsSubAction() const override;
	bool IsSubActionEnd() const override;
	bool IsMovedHotbarLeft() const override;
	bool IsMovedHotbarRight() const override;
	bool IsInteractBegin() const override;
	bool IsInteract() const override;
	bool IsInteractEnd() const override;
	bool IsJumpBegin() const override;
	bool IsJump() const override;
	bool IsJumpEnd() const override;

private:
	AbstructInputSystem& abstructInputSystem_;
};
