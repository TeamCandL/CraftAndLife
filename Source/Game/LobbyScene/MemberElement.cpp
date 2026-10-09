#include "pch\pch.h"
#include "MemberElement.h"

void MemberElement::Start()
{
}

void MemberElement::SetName(const std::string_view _name)
{
	text.content = _name;
}
