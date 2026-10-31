#include "pch\pch.h"
#include "CraftUI.h"
#include <Game/PlayScene/PlayerBag.h>
#include <ranges>

void CraftUI::Start()
{
	const auto& allItemList = Get<ItemSystem>().Data().allItemList;
	auto itr                = std::find_if(
        allItemList.begin(),
        allItemList.end(),
        [](const ItemAllInfo& _info)
        {
            return _info._isCraftable;
        });
	if (itr != allItemList.end())
	{
		selectedItemId = itr->_id;
	}
}

ItemId CraftUI::Update(
    PlayerBag& _playerBag)
{
	const auto& allItemList = Get<ItemSystem>().Data().allItemList;
	std::vector<ItemAllInfo> craftableItems;
	for (const ItemAllInfo& info : allItemList | std::views::filter(
	                                                 [](const ItemAllInfo& _info)
	                                                 {
		                                                 return _info._isCraftable;
	                                                 }))
	{
		craftableItems.emplace_back(info);
	}

	ImGui::Begin("CraftUI_List(Test)");
	for (auto& craftableItem : craftableItems)
	{
		bool selected       = (selectedItemId == craftableItem._id);
		ItemId requiremntId = craftableItem._id;
		auto itr            = std::find_if(
            allItemList.begin(),
            allItemList.end(),
            [requiremntId](const ItemAllInfo& _info)
            {
                return requiremntId == _info._id;
            });
		if (itr == allItemList.end())
			continue;

		if (ImGui::Selectable(itr->_name.c_str(), selected))
		{
			selectedItemId = craftableItem._id;
		}
	}
	ImGui::End();

	ImGui::Begin("CraftUI_Preview");

	auto itemInfo     = Get<ItemSystem>().GetInfo(selectedItemId);
	auto requirements = _playerBag.GetCraftRequirements(selectedItemId);

	ImGui::Text("Item: %s", itemInfo._name.c_str());
	ImGui::Image((void*)itemInfo._imageId.Get().pSRV.Get(), ImVec2(64, 64));
	ImGui::Separator();

	for (auto& requirement : requirements)
	{
		ImGui::PushID(requirement.id);

		ImGui::Text("%s", Get<ItemSystem>().GetInfo(requirement.id)._name.c_str());
		ImGui::Text("所持数: %d", requirement.ownedCount);
		ImGui::Text("必要数 %d", requirement.requiredCount);
		ImGui::Separator();

		ImGui::PopID();
	}
	bool canCraft = std::all_of(
	    requirements.begin(),
	    requirements.end(),
	    [](const CraftMaterialRequirement& _requirement)
	    {
		    return _requirement.isSufficient;
	    });

	ItemId resultItemId = INVALID_ITEM_ID;
	ImGui::BeginDisabled(canCraft == false);

	if (ImGui::Button("TryCraft"))
	{
		resultItemId = selectedItemId;
	}
	ImGui::EndDisabled();
	ImGui::End();

	return resultItemId;
}
