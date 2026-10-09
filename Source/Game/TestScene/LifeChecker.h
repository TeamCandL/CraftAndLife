#pragma once
#include <Game/Game.h>

struct LifeCheckerObjData
{
	LifeCheckerObjData(std::string_view _name) :
		name{ _name }
	{
	}

	std::string name{};
};

struct LifeCheckerObj : ObjectBase<LifeCheckerObjData>
{
	using ObjectBase<LifeCheckerObjData>::ObjectBase;

	void Start() override { VCU_LOG("スタート{}", name); }
	void OnDestroy() override { VCU_LOG("デストロイ{}", name); }

	inline void SetName(const std::string_view _name)
	{
		name = _name;
	}
};


/// @brief インスタンスの生存期間をログに表示するチェッククラス
class LifeChecker
{
public:
	inline LifeChecker()
	{
		VCU_LOG("Construct:{}", name_);
	}

	inline ~LifeChecker()
	{
		VCU_LOG("Destruct:{}", name_);
	}

	inline void SetName(const std::string_view _name)
	{
		name_ = _name;
	}

private:
	std::string name_{};
};
