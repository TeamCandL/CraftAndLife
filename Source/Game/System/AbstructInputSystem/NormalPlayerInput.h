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
	virtual ~NormalPlayerInput() = default;

	// IPlayerInput を介して継承されました
	virtual Vec2 GetMoveBody() const override;
	virtual Vec2 GetMoveCamera() const override;
	virtual float GetActionTrigger() const override;
	virtual float GetSubActionTrigger() const override;
	virtual bool IsActionBegin() const override;
	virtual bool IsAction() const override;
	virtual bool IsActionEnd() const override;
	virtual bool IsSubActionBegin() const override;
	virtual bool IsSubAction() const override;
	virtual bool IsSubActionEnd() const override;
	virtual bool IsMovedHotbarLeft() const override;
	virtual bool IsMovedHotbarRight() const override;
	virtual bool IsInteractBegin() const override;
	virtual bool IsInteract() const override;
	virtual bool IsInteractEnd() const override;
	virtual bool IsJumpBegin() const override;
	virtual bool IsJump() const override;
	virtual bool IsJumpEnd() const override;
	virtual bool IsMessageBegin() const override;

	/// @brief 更新処理
	virtual void Update() override {}

  protected:
	AbstructInputSystem& abstructInputSystem_;
};
