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
	~NormalUIInput() = default;

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
