#include "pch\pch.h"
#include "SelfInfo.h"

#include <Game/Manager/SessionManager.h>
#include "../PlayerRoleType.h"

void SelfInfo::Start() {}

void SelfInfo::Update()
{
	// セッション情報から自分の役職を取得
	const auto& status = Get<SessionManager>().RefStatus();

	// 自分の名前を探す
	auto nameIt = std::ranges::find(status.memberNames, status.name);

	if (nameIt == status.memberNames.end())
	{
		return;
	}

	// 自分のプレイヤーIDを取得
	const size_t index = std::distance(status.memberNames.begin(), nameIt);

	if (index >= status.memberId.size())
	{
		return;
	}

	const int32_t playerId = status.memberId[index];

	// 自分のROLEを取得
	auto roleIt = status.memberRoles.find(playerId);

	if (roleIt == status.memberRoles.end())
	{
		return;
	}

	const int32_t role = roleIt->second;

	// 有効な役職か確認
	if (role < PlayerRoleType_Healer || role >= PlayerRoleType_Max)
	{
		return;
	}

	// 役職が変わっていなければ画像を変更しない
	if (role == currentRole)
	{
		return;
	}

	// 役職に応じて背景画像を変更
	switch (role)
	{
	case PlayerRoleType_Healer:
		image.texture = Resource<Texture>{ "Image/UI/Healer_info.png" };
		break;

	case PlayerRoleType_Tank:
		image.texture = Resource<Texture>{ "Image/UI/Tank_info.png" };
		break;

	case PlayerRoleType_Archer:
		image.texture = Resource<Texture>{ "Image/UI/Fighter_info.png" };
		break;

	case PlayerRoleType_Fighter:
		image.texture = Resource<Texture>{ "Image/UI/Archer_info.png" };
		break;
	}

	currentRole = role;
}