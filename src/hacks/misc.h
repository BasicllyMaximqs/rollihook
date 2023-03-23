#pragma once
#include "../../config.h"
#include "../../gui.h"
class CUserCmd;



namespace hacks
{

	inline bool currently_planted = false;

	inline std::int32_t plant_idx;

	void sendBuylog(IGameEvent* eevent) noexcept;

	void RunNightMode() noexcept;

	void RunBunnyHop(CUserCmd* cmd) noexcept;

	void RunAntiAim(CUserCmd* cmd) noexcept;

	void RunRagebot(CUserCmd* cmd) noexcept;
	void KillSay(IGameEvent* event) noexcept;

	void SendHitsound(IGameEvent* event) noexcept;

	void setClanTag(IGameEvent* event) noexcept;
	void bombRevealPlant(IGameEvent* eevent) noexcept;

	void removeVisualRecoil(ClientFrameStage_t stage) noexcept;
}
