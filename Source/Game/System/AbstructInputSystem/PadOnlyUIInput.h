#pragma once
#include "IUIInput.h"
#include "../AbstructInputSystem.h"

class PadOnlyUIInput : public IUIInput
{
  public:
	/// @brief コンストラクタ
	/// @param _abstructInputSystem 抽象入力システムの参照ポインタ
	PadOnlyUIInput(AbstructInputSystem& _abstructInputSystem);
	/// @brief デストラクタ
	~PadOnlyUIInput() = default;

	// IUIInput を介して継承されました
	bool IsMovedUp() override;
	bool IsMovedDown() override;
	bool IsMovedLeft() override;
	bool IsMovedRight() override;
	bool IsSelected() const override;
	bool IsChanneled() const override;

	/// @brief 更新処理
	virtual void Update() override {}

  private:
	AbstructInputSystem& abstructInputSystem_;
};
