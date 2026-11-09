#pragma once
#include <Game/Game.h>
#include <Game/PlayScene/ItemSystem.h>
#include <Game/PlayScene/PlayerBag.h>
#include <Game/PlayScene/Player/PlayerJobType.h>
#include <Game/UI/ImageObject.h>
#include <Game/UI/TextObject.h>
#include <nlohmann-json/json.hpp>
#include <unordered_map>
#include <span>

struct PlayerBag;

inline void from_json_UITransform(
    const nlohmann::json& _j,
    UITransform& _transform)
{
	_j.at("x").get_to(_transform.position.x);
	_j.at("y").get_to(_transform.position.y);
	_j.at("width").get_to(_transform.scale.x);
	_j.at("height").get_to(_transform.scale.y);
}

struct RecipeMaterial
{
	GameObject objRecipeMaterialElementImage{};
	GameObject objRecipeMaterialElementText{};
	GameObject objRecipeHeldElement{};
	GameObject objRecipeRequiredElement{};
	GameObject objRecipeSlashElement{};
	ItemId craftItemId{ EMPTY_ITEM_ID };
};

struct CraftUIData
{
	CraftUIData(GameObject _objPlayer);

  protected:
	ItemId selectedItemId{ EMPTY_ITEM_ID };
	GameObject objListBox{};
	Resource<Font> font{ FontLoadConfig{}
		                     .WithFile("Font/Stick/Stick-Regular.ttf")
		                     .WithFamilyName(L"Stick")
		                     .WithFontSize(32)
		                     .WithFontAlignment(FontAlignment_Center) };

	Resource<ResourceType::Texture> backgroundTexture{ "Image/UI/craft_ui_background.png" };
	Resource<ResourceType::Texture> craftPreviewbackgroundTexture{ "Image/UI/craft_preview.png" };

	GameObject objCraftBackgroundImage{};

	GameObject objSelectedItemImage{};
	GameObject objSelectedItemText{};
	GameObject objSelectedItemBackground{};
	// レシピ画面
	GameObject objRecipeMaterialLabel{};
	GameObject objRecipeHeldLabel{};
	GameObject objRecipeRequiredLabel{};
	GameObject objRecipeSlashLabel{};

	Vec2 materialElementImageBegin{};
	Vec2 materialElementImageSize{};
	Vec2 materialElementNameBegin{};
	Vec2 materialElementHeldBegin{};
	Vec2 materialElementRequiredBegin{};
	Vec2 materialElementSlashBegin{};

	std::vector<ItemAllInfo> craftableItems;
	std::unordered_map<ItemId, std::vector<RecipeMaterial>> recipeUIMap;

	GameObject objPlayer{};
	PlayerJobType playerJobType{ PlayerJobType_Max };
};

struct CraftUI : ObjectBase<CraftUIData>
{
	using ObjectBase<CraftUIData>::ObjectBase;
	void Start() override;
	void Update() override;
	ItemId UpdateUI();
	void SetActiveAll(const bool _isActive);

  private:
	void SetTextObjectFromJson(const nlohmann::json& _json, const GameObject& _obj);
	void SetImageObjectFromJson(const nlohmann::json& _json, const GameObject& _obj);
	void UpdateRequirementUI(
	    const std::vector<CraftMaterialRequirement>& _requirements,
	    std::vector<RecipeMaterial>& _recipeMaterial);
	void SetRecipeUIActive(std::vector<RecipeMaterial>& _recipes, const bool _isActive);

	void DrawDebugItemAddWindow();
};
