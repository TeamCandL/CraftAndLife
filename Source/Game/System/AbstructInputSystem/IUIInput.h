#pragma once
#include <Game/Game.h>
#include "IInput.h"

/// @brief UIの入力抽象化インタフェース
class IUIInput : public IInput
{
  public:
	/// @brief コンストラクタ
	IUIInput() = default;
	/// @brief デストラクタ
	virtual ~IUIInput() = default;

	/// @brief 上方向への移動があったか
	/// @return あった true / false
	virtual bool IsMovedUp() = 0;
	/// @brief 下方向への移動があったか
	/// @return あった true / false
	virtual bool IsMovedDown() = 0;
	/// @brief 左方向への移動があったか
	/// @return あった true / false
	virtual bool IsMovedLeft() = 0;
	/// @brief 右方向への移動があったか
	/// @return あった true / false
	virtual bool IsMovedRight() = 0;
	/// @brief 選択されたか
	/// @return 選択された true / false
	virtual bool IsSelected() const = 0;
	/// @brief キャンセルされたか
	/// @return キャンセルされた true / false
	virtual bool IsChanneled() const = 0;

  public:
	inline static const float AUTO_INPUT_WAIT_THRESHOLD_SEC{ 0.5f };
	inline static const float AUTO_INPUT_DISTANCE_SEC{ 0.1f };
};
