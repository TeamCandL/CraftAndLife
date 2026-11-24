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
	virtual bool IsMovedUp() override;
	virtual bool IsMovedDown() override;
	virtual bool IsMovedLeft() override;
	virtual bool IsMovedRight() override;
	virtual bool IsSelected() const override;
	virtual bool IsChanneled() const override;
	virtual bool IsMovedLeftTab() override;
	virtual bool IsMovedRightTab() override;

	/// @brief 更新処理
	virtual void Update() override;

  private:
	/// @brief 自動パルス入力の更新処理
	/// @param _directionType 方向
	/// @param _isOn 入力があるか true / false
	/// @return 自動パルス入力の出力
	bool UpdateAutoInput(const InputDirectionType _directionType, const bool _isOn);

  protected:
	AbstructInputSystem& abstructInputSystem_;

	std::array<float, InputDirectionType_Max> autoInputWaitTimeLefts{
		AUTO_INPUT_WAIT_THRESHOLD_SEC, AUTO_INPUT_WAIT_THRESHOLD_SEC, AUTO_INPUT_WAIT_THRESHOLD_SEC,
		AUTO_INPUT_WAIT_THRESHOLD_SEC, AUTO_INPUT_WAIT_THRESHOLD_SEC, AUTO_INPUT_WAIT_THRESHOLD_SEC,
	};
	std::array<float, InputDirectionType_Max> autoInputTimeLefts{
		AUTO_INPUT_DISTANCE_SEC, AUTO_INPUT_DISTANCE_SEC, AUTO_INPUT_DISTANCE_SEC,
		AUTO_INPUT_DISTANCE_SEC, AUTO_INPUT_DISTANCE_SEC, AUTO_INPUT_DISTANCE_SEC,
	};
	std::array<bool, InputDirectionType_Max> autoInputValues{
		false, false, false, false, false, false,
	};
};
