#include "globals.h"

// expose interfaces to this file
#include "interfaces.h"

void globals::UpdateLocalPlayer() noexcept
{
	// get local player index from engine
	localPlayerIdx = interfaces::engine->GetLocalPlayerIndex();
	// get local player entity from client entity list
	localPlayer = interfaces::entityList->GetEntityFromIndex(localPlayerIdx);
}
