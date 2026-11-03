#pragma once
#include <Game/Game.h>
#include "Harvestable.h"

struct HarvestableControllerData
{
	std::vector<GameObject> objs{};
};

struct HarvestableController : ObjectBase<HarvestableControllerData>
{
	/// @brief ハーベスタブルオブジェクトを追加する
	/// @param _type ハーベスタブルオブジェクトの種類
	/// @param _position 座標
	/// @param _scale 大きさ
	void AddHarvestable(HarvestableType _type, Vec3 _position, float _scale = 5.0f);

	/// @brief ローカル上で破壊されているハーベスタブルオブジェクトのIdを取得
	/// @return オプション id | none
	std::optional<uint32_t> FindToLocalBokenIds();

	/// @brief 指定のハーベスタブルオブジェクトを破壊する
	/// @param _id Id
	void BreakAt(const uint32_t _id);
};
