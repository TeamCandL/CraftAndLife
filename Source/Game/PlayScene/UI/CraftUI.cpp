#include "pch\pch.h"
#include "CraftUI.h"
#include <Game/System/AbstructInputSystem.h>
#include <Game/PlayScene/UI/ListBox.h>
#include <Game/PlayScene/Player.h>
#include <Game/PlayScene/EntitySystem.h>
#include <Game/Manager/SessionManager.h>

#include "HotBar.h"
#include <ranges>
#include <format>
#include <fstream>

namespace
{

	std::string GetJobName(
	    PlayerJobType _jobType)
	{
		switch (_jobType)
		{
		case PlayerJobType_Healer:  ///< 回復使い
			return "ヒーラー";
			break;
		case PlayerJobType_Tank:  ///< 盾使い
			return "タンク";
			break;
		case PlayerJobType_Fighter:  ///< 大剣使い
			return "ファイター";
			break;
		case PlayerJobType_Archer:  ///< 弓使い
			return "アーチャー";
			break;
		case PlayerJobType_Max:  ///< 最大数
		default:
			return "無効";
			break;
		}
	}
}  // namespace

void CraftUI::Start()
{
	bool filterCraftableItemOnJob{ Get<Player>(objPlayer).Data().filterAddItemOnJob };
	playerJobType = Get<Player>(objPlayer).Data().jobType;

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

	for (const ItemAllInfo& info : allItemList | std::views::filter(
	                                                 [this, filterCraftableItemOnJob](const ItemAllInfo& _info)
	                                                 {
		                                                 // 制作可能アイテム かつ そのジョブがクラフト可能
		                                                 bool playerCanCraft =
		                                                     _info.canUseJobType == PlayerJobType_Max ||
		                                                     _info.canUseJobType == playerJobType ||
		                                                     filterCraftableItemOnJob == false;
		                                                 return _info._isCraftable && playerCanCraft;
	                                                 }))
	{
		craftableItems.push_back(info);
	}

	objListBox           = GetScene().Instantiate<UI::ListBox>();
	UI::ListBox& listBox = Get<UI::ListBox>(objListBox);

	/// ---------------
	/// リストボックス
	/// ---------------
	Vec2 recipeListBegin{ 429, 195 };
	int recipeElementHeight{ 40 };
	for (int i = 0; i < craftableItems.size(); i++)
	{
		Vec2 begin{ recipeListBegin + Vec2{ 0, recipeElementHeight * i } };
		GameObject objListElement = GetScene().Instantiate<UI::ListElement>(
		    "Image/EmptyItem.png",
		    "Image/UI/selected_recipe_background.png",
		    craftableItems[i]._name,
		    std::to_string(craftableItems[i]._id),
		    begin,
		    Vec2{ 271, 40 },
		    font);
		listBox.PushBackElement(Get<UI::ListElement>(objListElement));
	}
	std::ifstream i("UI/CraftUI.json");
	nlohmann::json j;
	i >> j;

	nlohmann::json listBoxJson = j["craftable_list_group"];
	// リストボックス背景の画像、座標
	from_json_UITransform(listBoxJson, listBox.Data().transform);
	listBox.Data().sprite.texture = backgroundTexture;

	/// ---------------------------
	/// 選択中のアイテムの画面
	/// ---------------------------
	nlohmann::json selectedItemJson = j["selected_item_frame"]["children"];
	auto& scene                     = GetScene();
	objSelectedItemBackground       = scene.Instantiate<ImageObject>(
        Resource<ResourceType::Texture>("Image/UI/selected_item_background.png"),
        Vec2{ 700, 241 },
        Vec2{ 619, 186 },
        2);
	{
		nlohmann::json j = selectedItemJson["background"];
		SetImageObjectFromJson(j, objSelectedItemBackground);
	}

	objSelectedItemImage = scene.Instantiate<ImageObject>(
	    Resource<ResourceType::Texture>("Image/EmptyItem.png"),
	    Vec2{ 789, 270 },
	    Vec2{ 128, 128 },
	    3);
	{
		SetImageObjectFromJson(selectedItemJson["icon"], objSelectedItemImage);
	}
	FontLoadConfig config;
	config.fontSize = 64;

	Resource<ResourceType::Font> font{ config };

	objSelectedItemText = scene.Instantiate<TextObject>(Vec2{ 1089, 280 }, Vec2{ 64, 64 }, 3, font);
	{
		SetTextObjectFromJson(selectedItemJson["name"], objSelectedItemText);
	}

	objSelectedItemOnlyJobLabel = scene.Instantiate<TextObject>(Vec2{ 1089, 280 }, Vec2{ 64, 64 }, 3, font);
	{
		SetTextObjectFromJson(selectedItemJson["only_job_label"], objSelectedItemOnlyJobLabel);
	}

	/// --------------------
	/// 充足状況画面
	/// --------------------
	nlohmann::json recipeGroupJson = j["recipe_group"]["children"];
	objRecipeMaterialLabel         = scene.Instantiate<TextObject>(Vec2{ 1089, 280 }, Vec2{ 64, 64 }, 3);
	SetTextObjectFromJson(recipeGroupJson["material_label"], objRecipeMaterialLabel);

	objRecipeHeldLabel = scene.Instantiate<TextObject>(Vec2{ 913, 454 }, Vec2{ 192, 46 }, 3);
	SetTextObjectFromJson(recipeGroupJson["held_label"], objRecipeHeldLabel);
	objRecipeRequiredLabel = scene.Instantiate<TextObject>(Vec2{ 1139, 452 }, Vec2{ 64, 64 }, 3);
	SetTextObjectFromJson(recipeGroupJson["required_label"], objRecipeRequiredLabel);

	objRecipeSlashLabel = scene.Instantiate<TextObject>(Vec2{ 1115, 454 }, Vec2{ 32, 32 }, 3);
	SetTextObjectFromJson(recipeGroupJson["slash_label"], objRecipeSlashLabel);

	// 個々の素材のUI
	{
		nlohmann::json j            = recipeGroupJson["material_element_first_icon"];
		materialElementImageBegin.x = j.at("x").get<int>();
		materialElementImageBegin.y = j.at("y").get<int>();
		materialElementImageSize.x  = j.at("width").get<int>();
		materialElementImageSize.y  = j.at("height").get<int>();
	}
	{
		nlohmann::json j           = recipeGroupJson["material_element_first_name"];
		materialElementNameBegin.x = j.at("x").get<int>();
		materialElementNameBegin.y = j.at("y").get<int>();
	}
	{
		nlohmann::json j           = recipeGroupJson["material_element_first_held"];
		materialElementHeldBegin.x = j.at("x").get<int>();
		materialElementHeldBegin.y = j.at("y").get<int>();
	}
	{
		nlohmann::json j               = recipeGroupJson["material_element_first_required"];
		materialElementRequiredBegin.x = j.at("x").get<int>();
		materialElementRequiredBegin.y = j.at("y").get<int>();
	}
	{
		nlohmann::json j            = recipeGroupJson["material_element_first_slash"];
		materialElementSlashBegin.x = j.at("x").get<int>();
		materialElementSlashBegin.y = j.at("y").get<int>();
	}

	objCraftBackgroundImage = GetScene().Instantiate<ImageObject>(craftPreviewbackgroundTexture, Vec2{}, Vec2{}, 0);
	SetImageObjectFromJson(j["craft_preview"], objCraftBackgroundImage);
	for (int i = 0; i < craftableItems.size(); i++)
	{
		ItemAllInfo& info                = craftableItems[i];
		std::vector<CraftInfo> craftInfo = info._craftInfo;
		std::vector<RecipeMaterial> materials;
		for (int iCraftInfo = 0; iCraftInfo < craftInfo.size(); iCraftInfo++)
		{
			// 素材ごとのテキストのオフセット。
			int textOffset = 55;
			RecipeMaterial recipeUI{ scene.Instantiate<ImageObject>(
				                         // 素材の画像
				                         Get<ItemSystem>().GetInfo(craftInfo[iCraftInfo]._id)._imageId,
				                         // 素材画像
				                         materialElementImageBegin +
				                             Vec2{ 0.0f, (23 + materialElementImageSize.y) * iCraftInfo },
				                         materialElementImageSize,
				                         3),
				                     // 素材名
				                     scene.Instantiate<TextObject>(
				                         materialElementNameBegin + Vec2{ 0, (textOffset + 32) * iCraftInfo },
				                         Vec2{ 32, 32 },
				                         3),
				                     // 所持数テキスト
				                     scene.Instantiate<TextObject>(
				                         materialElementHeldBegin + Vec2{ 0, (textOffset + 32) * iCraftInfo },
				                         Vec2{ 32, 32 },
				                         3),
				                     // 必要数テキスト
				                     scene.Instantiate<TextObject>(
				                         materialElementRequiredBegin + Vec2{ 0, (textOffset + 32) * iCraftInfo },
				                         Vec2{ 32, 32 },
				                         3),
				                     // スラッシュのテキスト
				                     scene.Instantiate<TextObject>(
				                         materialElementSlashBegin + Vec2{ 0, (textOffset + 32) * iCraftInfo },
				                         Vec2{ 32, 32 },
				                         3),
				                     // 素材のアイテムID
				                     craftInfo[iCraftInfo]._id };
			materials.push_back(recipeUI);
		}
		ItemId id = info._id;
		recipeUIMap.insert({ id, materials });
	}
}

void CraftUI::Update()
{
#ifdef USE_IMGUI
	DrawDebugItemAddWindow();
#endif
	PlayerData& playerData = Get<Player>(objPlayer).Data();
	if (playerData.helthState == HelthState_Dead)
	{
		playerData.isOpenCraftUI = false;
	}
	if (playerData.isOpenCraftUI == false)
	{
		SetActiveAll(false);
		return;
	}
	SetActiveAll(true);

	ItemId resultItemId = UpdateUI();
	if (resultItemId == EMPTY_ITEM_ID)
		return;
	PlayerBag& playerBag = Get<PlayerBag>(Get<Player>(objPlayer).Data().objPlayerBag);

	ItemInfo info = Get<ItemSystem>().GetInfo(selectedItemId);

	// クラフトを試みて、成功した場合
	if (playerBag.TryCraft(resultItemId))
	{
		Get<Player>(objPlayer).TryAddItem(resultItemId);
	}
}

ItemId CraftUI::UpdateUI()
{
	PlayerBag& _playerBag = Get<PlayerBag>(Get<Player>(objPlayer).Data().objPlayerBag);
	auto padInput         = Get<AbstructInputSystem>().GetPadInputSystem();

	const auto& allItemList = Get<ItemSystem>().Data().allItemList;

	UI::ListBox& listBox = Get<UI::ListBox>(objListBox);

	selectedItemId = std::stoi(listBox.GetSelectionData());

	// 選択中アイテムの画像、テキストを描画
	auto itemInfo                                               = Get<ItemSystem>().GetInfo(selectedItemId);
	Get<ImageObject>(objSelectedItemImage).Data().image.texture = itemInfo._imageId;

	TextObjectData& selectedTextObjectData   = Get<TextObject>(objSelectedItemText).Data();
	selectedTextObjectData.text.content      = itemInfo._name;
	selectedTextObjectData.transform.scale.x = selectedTextObjectData.text.content.size() * 64;

	TextObjectData& selectedOnlyJobLabelData = Get<TextObject>(objSelectedItemOnlyJobLabel).Data();
	auto& status                             = Get<SessionManager>().RefStatus();
	PlayerData& playerData                   = Get<Player>(objPlayer).Data();
	PlayerJobType playerJobType              = playerData.jobType;
	if (itemInfo.canUseJobType == playerJobType)
	{
		if (playerData.isOpenCraftUI)
		{
			Get<TextObject>(objSelectedItemOnlyJobLabel).SetActive(true);
		}
		selectedOnlyJobLabelData.text.content      = std::format("{}専用!", GetJobName(playerJobType));
		selectedOnlyJobLabelData.transform.scale.x = selectedOnlyJobLabelData.text.content.size() * 32;
	}
	else
	{
		if (playerData.isOpenCraftUI)
		{
			Get<TextObject>(objSelectedItemOnlyJobLabel).SetActive(false);
		}
		selectedOnlyJobLabelData.text.content = "";
	}

	ItemId resultItemId = EMPTY_ITEM_ID;

	/// ---------------
	/// レシピの描画
	/// ---------------

	// いったん、すべてのレシピを非アクティブにする
	for (auto& [id, recipe] : recipeUIMap)
	{
		SetRecipeUIActive(recipe, false);
	}

	// 選択中のアイテムのレシピを取得
	auto itr = recipeUIMap.find(selectedItemId);
	if (itr == recipeUIMap.end())
		return resultItemId;

	// 選択中のアイテムのレシピだけアクティブ
	SetRecipeUIActive(itr->second, true);
	auto requirements = _playerBag.GetCraftRequirements(selectedItemId);
	// レシピのUIを更新
	UpdateRequirementUI(requirements, itr->second);

	// ボタンが押されているなら、選択中のIDを返す
	if (listBox.IsPushed())
	{
		resultItemId = selectedItemId;
	}

	return resultItemId;
}

void CraftUI::SetActiveAll(
    const bool _isActive)
{
	auto& collection = GetScene().RefGameObjectCollection();
	collection.At<IObject>(objCraftBackgroundImage).SetActive(_isActive);
	collection.At<IObject>(objSelectedItemImage).SetActive(_isActive);
	collection.At<IObject>(objSelectedItemText).SetActive(_isActive);
	collection.At<IObject>(objSelectedItemBackground).SetActive(_isActive);
	collection.At<IObject>(objSelectedItemOnlyJobLabel).SetActive(_isActive);

	collection.At<IObject>(objRecipeMaterialLabel).SetActive(_isActive);
	collection.At<IObject>(objRecipeHeldLabel).SetActive(_isActive);
	collection.At<IObject>(objRecipeRequiredLabel).SetActive(_isActive);
	collection.At<IObject>(objRecipeSlashLabel).SetActive(_isActive);

	// すべてのレシピを非アクティブにする
	for (auto& [id, recipe] : recipeUIMap)
	{
		SetRecipeUIActive(recipe, _isActive);
	}

	Get<UI::ListBox>(objListBox).SetActiveAll(_isActive);
	collection.At<IObject>(objListBox).SetActive(_isActive);
}

void CraftUI::SetTextObjectFromJson(
    const nlohmann::json& _json,
    const GameObject& _obj)
{
	from_json(_json, Get<TextObject>(_obj).Data());
}

void CraftUI::SetImageObjectFromJson(
    const nlohmann::json& _json,
    const GameObject& _obj)
{
	from_json(_json, Get<ImageObject>(_obj).Data());
}

void CraftUI::UpdateRequirementUI(
    const std::vector<CraftMaterialRequirement>& _requirements,
    std::vector<RecipeMaterial>& _recipeMaterial)
{
	for (int i = 0; i < _requirements.size(); i++)
	{
		const CraftMaterialRequirement& requirement = _requirements[i];
		RecipeMaterial& _recipe                     = _recipeMaterial[i];

		ItemInfo itemInfo = Get<ItemSystem>().GetInfo(requirement.id);
		{
			ImageObjectData& imageObjectData = Get<ImageObject>(_recipe.objRecipeMaterialElementImage).Data();
			imageObjectData.image.texture    = itemInfo._imageId;
		}
		{
			TextObjectData& textObjectData   = Get<TextObject>(_recipe.objRecipeMaterialElementText).Data();
			textObjectData.text.content      = itemInfo._name;
			textObjectData.transform.scale.x = textObjectData.text.content.size() * 32;
		}
		{
			TextObjectData& textObjectData   = Get<TextObject>(_recipe.objRecipeHeldElement).Data();
			textObjectData.text.content      = std::to_string(requirement.ownedCount);
			textObjectData.transform.scale.x = textObjectData.text.content.size() * 32;
		}
		{
			TextObjectData& textObjectData   = Get<TextObject>(_recipe.objRecipeRequiredElement).Data();
			textObjectData.text.content      = std::to_string(requirement.requiredCount);
			textObjectData.transform.scale.x = textObjectData.text.content.size() * 32;
		}
		{
			TextObjectData& textObjectData   = Get<TextObject>(_recipe.objRecipeSlashElement).Data();
			textObjectData.text.content      = "/";
			textObjectData.transform.scale.x = 32;
		}
	}
}

void CraftUI::SetRecipeUIActive(
    std::vector<RecipeMaterial>& _recipes,
    const bool _isActive)
{
	for (auto& recipe : _recipes)
	{
		Get<ImageObject>(recipe.objRecipeMaterialElementImage).SetActive(_isActive);
		Get<TextObject>(recipe.objRecipeMaterialElementText).SetActive(_isActive);
		Get<TextObject>(recipe.objRecipeHeldElement).SetActive(_isActive);
		Get<TextObject>(recipe.objRecipeRequiredElement).SetActive(_isActive);
		Get<TextObject>(recipe.objRecipeSlashElement).SetActive(_isActive);
	}
}

void CraftUI::DrawDebugItemAddWindow()
{
	auto& allItemList = Get<ItemSystem>().Data().allItemList;
	auto& status      = Get<SessionManager>().RefStatus();

	// デバッグ用のUI
	ImGui::Begin(std::format("Debug_Item_Add:{}", status.name.c_str()).c_str());

	for (auto& itemAllInfo : allItemList)
	{
		if (itemAllInfo._imageId.IsNull() || itemAllInfo._model.IsNull())
			continue;

		ImGui::Text("%d", itemAllInfo._id);
		ImGui::SameLine();
		ImGui::Text("%s", itemAllInfo._name.c_str());
		ImGui::SameLine();
		ImGui::Image((void*)itemAllInfo._imageId.Get().pSRV.Get(), ImVec2{ 32, 32 });
		ImGui::SameLine();

		ImGui::PushID(itemAllInfo._id);
		if (ImGui::Button("Add"))
		{
			Get<Player>(objPlayer).TryAddItem(itemAllInfo._id);
		}
		ImGui::PopID();
	}

	ImGui::End();
}

CraftUIData::CraftUIData(
    GameObject _objPlayer) :
    objPlayer{ _objPlayer }
{
}
