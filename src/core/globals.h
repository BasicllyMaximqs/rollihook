#pragma once
#include <cstdint>
class CEntity;
namespace globals
{
	inline CEntity* localPlayer = nullptr;
	
	inline std::int32_t localPlayerIdx;

	inline std::int32_t scr_x, scr_y;

	// update the local player pointer
	void UpdateLocalPlayer() noexcept;
}
