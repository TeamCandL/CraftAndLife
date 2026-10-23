#include "pch\pch.h"
#include "ItemSystem.h"

void ItemSystemData::GetInfo(int32_t id_)
{
	return {allItemList[id_]._imageId};
}
