#pragma once

enum JoinError : int32_t
{
	NameDuplicate = 0,  // 名前の重複
	LobbyFull     = 1   // ロビーが満員
};