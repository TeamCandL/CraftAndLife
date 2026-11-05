#include "pch\pch.h"
#include "SessionStateReader.h"
#include "../SessionManager.h"

SessionStateReader::SessionStateReader(
    const SessionManager& _sessionManager) :
    sessionManager_{ _sessionManager }
{
}