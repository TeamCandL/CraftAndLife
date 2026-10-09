#include "pch\pch.h"
#include "MemberList.h"
#include "MemberElement.h"
#include <Game/Manager/SessionManager.h>

void MemberList::Start()
{
	image.texture = texture;

	// リストの文字列要素を追加していく
	memberObjElements.resize(MEMBER_LIST_SIZE);
	for (size_t i = 0; i < MEMBER_LIST_SIZE; i++)
	{
		memberObjElements[i] = GetScene().Instantiate<MemberElement>(
			Vec2{ transform.position.x, transform.position.y + ELEMENT_SCALE_PIX.y * i },
			ELEMENT_SCALE_PIX);
	}
}

void MemberList::UpdateMemberList()
{
	const std::vector<std::string>& memberNames = Get<SessionManager>().RefStatus().memberNames;

	VCU_LOG("メンバー更新");

	for (size_t i = 0; i < memberObjElements.size(); i++)
	{
		if (i < memberNames.size())
		{
			VCU_LOG("[{}]:{}", i, memberNames[i]);
			Get<MemberElement>(memberObjElements[i]).SetName(memberNames[i]);
		}
		else
		{
			Get<MemberElement>(memberObjElements[i]).SetName("-- 空き --");
		}
	}
}

void MemberList::RemoveMember(const size_t _index)
{
	VCU_ASSERT((_index < memberNames.size()) && "範囲外アクセス");

	memberNames[_index] = "-- 空き --";  // メンバーなしにする
	Get<MemberElement>(memberObjElements[_index]).SetName(memberNames[_index]);
}

void MemberList::SetMember(const size_t _index, const std::string_view _name)
{
	VCU_ASSERT((_index < memberNames.size()) && "範囲外アクセス");

	memberNames[_index] = _name;
	Get<MemberElement>(memberObjElements[_index]).SetName(memberNames[_index]);
}
