#pragma once
#include "IUIInput.h"
#include "../AbstructInputSystem.h"

class TutorialUIInput : public IUIInput
{
  public:
	/// @brief コンストラクタ
	/// @param _abstructInputSystem 抽象入力システムの参照ポインタ
	TutorialUIInput(AbstructInputSystem& _abstructInputSystem);
	/// @brief デストラクタ
	~TutorialUIInput() = default;

	// IUIInput を介して継承されました
	bool IsMovedUp() const override;
	bool IsMovedDown() const override;
	bool IsMovedLeft() const override;
	bool IsMovedRight() const override;
	bool IsSelected() const override;
	bool IsChanneled() const override;

  private:
	AbstructInputSystem& abstructInputSystem_;
};
