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

	/// @brief 更新処理
	virtual void Update() override;

  protected:
	inline static const float AUTO_INPUT_WAIT_THRESHOLD_SEC{ 0.3f };
	inline static const float AUTO_INPUT_DISTANCE_SEC{ 0.3f };

	using DirectionType = int32_t;
	enum DirectionType_ : DirectionType
	{
		DirectionType_Up,
		DirectionType_Down,
		DirectionType_Left,
		DirectionType_Right,
		DirectionType_Max,
	};

	AbstructInputSystem& abstructInputSystem_;

	std::array<float, DirectionType_Max> autoInputWaitTimeLefts{
		AUTO_INPUT_WAIT_THRESHOLD_SEC,
		AUTO_INPUT_WAIT_THRESHOLD_SEC,
		AUTO_INPUT_WAIT_THRESHOLD_SEC,
		AUTO_INPUT_WAIT_THRESHOLD_SEC,
	};
	std::array<float, DirectionType_Max> autoInputTimeLefts{
		AUTO_INPUT_DISTANCE_SEC,
		AUTO_INPUT_DISTANCE_SEC,
		AUTO_INPUT_DISTANCE_SEC,
		AUTO_INPUT_DISTANCE_SEC,
	};
};
