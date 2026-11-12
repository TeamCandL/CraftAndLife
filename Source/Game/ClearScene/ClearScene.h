#pragma once
#include <Game/Game.h>
#include "ResultType.h"

struct ClearScene : SceneBase
{
	ClearScene(
	    ResultType _resultType) :
	    resultType(_resultType)
	{
	}

	void Start() override;

  private:
	ResultType resultType{ ResultType::Clear };
};