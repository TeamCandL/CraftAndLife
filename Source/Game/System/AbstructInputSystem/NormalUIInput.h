#pragma once
#include "IUIInput.h"
#include "../AbstructInputSystem.h"

class NormalUIInput : public IUIInput
{
  public:
	/// @brief コンストラクタ
	/// @param _abstructInputSystem 抽象入力システムの参照ポインタ
	NormalUIInput(AbstructInputSystem& _abstructInputSystem);
	/// @brief デストラクタ
	virtual ~NormalUIInput() = default;

	// IUIInput を介して継承されました
	virtual bool IsMovedUp() const override;
	virtual bool IsMovedDown() const override;
	virtual bool IsMovedLeft() const override;
	virtual bool IsMovedRight() const override;
	virtual bool IsSelected() const override;
	virtual bool IsChanneled() const override;

  protected:
	AbstructInputSystem& abstructInputSystem_;
};
