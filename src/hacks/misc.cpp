#include "misc.h"

// access global variables
#include "../core/globals.h"

// access interfaces
#include "../core/interfaces.h"

void hacks::RunNightMode() noexcept {

	ConVar* ambient = interfaces::cvar->findVar("mat_autoexposure_max_multiplier");
	if (config::map_color) {

		if (ambient->getFloat() == 1.0f) {
			ambient->SetValue(0.1f);
		}
	}
	else {
		if (ambient->getFloat() == 0.1f) {
			ambient->SetValue(1.0f);
		}
	}
}

void hacks::RunBunnyHop(CUserCmd* cmd) noexcept
{
	if (!config::bunnyhop)
		return;

	if (!globals::localPlayer)
		return;

	if (!interfaces::engine->IsInGame())
		return;

	if (!(globals::localPlayer->GetFlags() & CEntity::FL_ONGROUND))
		cmd->buttons &= ~CUserCmd::IN_JUMP;

	if (config::auto_strafe && (GetAsyncKeyState(VK_SPACE) && !(globals::localPlayer->GetFlags() & CEntity::FL_ONGROUND)))
	{
		if (cmd->mouseDeltaX < 0)
		{
			cmd->sideMove = config::anti_aim ? 450.f : -450.f;
		}

		if (cmd->mouseDeltaX > 0)
		{
			cmd->sideMove = config::anti_aim ? -450.f : 450.f;
		}

	}
}

void hacks::KillSay(IGameEvent* eevent) noexcept
{
	if (config::trashtalk && !strcmp(eevent->GetName(), "player_death")) //event name from the list
	{
		if (interfaces::engine->GetUserFromID(eevent->GetInt("attacker")) == globals::localPlayerIdx)
		{
			interfaces::engine->cmd_unrestricted("say 1 nn banane pwned by rollihook");
		}
	}
}

void hacks::removeVisualRecoil(ClientFrameStage_t stage) noexcept
{
	if (!globals::localPlayer->IsAlive())
		return;

	static CVector aimPunch;
	static CVector viewPunch;

	if (stage == ClientFrameStage_t::FRAME_RENDER_START) {
		aimPunch = globals::localPlayer->aimPunchAngle();
		viewPunch = globals::localPlayer->viewPunchAngle();

		if (config::RemoveAimPunch)
			globals::localPlayer->aimPunchAngle() = CVector{ };

		if (config::RemoveViewPunch)
			globals::localPlayer->viewPunchAngle() = CVector{ };

	}
	
}

void hacks::bombRevealPlant(IGameEvent* eevent) noexcept
{
	if (!strcmp(eevent->GetName(), "bomb_planted")) //event name from the list
	{
		currently_planted = true;
		plant_idx = eevent->GetInt("site");

	}
	if (!strcmp(eevent->GetName(), "bomb_exploded")) //event name from the list
	{
		currently_planted = false;
		plant_idx = -1;

	}
	if (!strcmp(eevent->GetName(), "round_start")) //event name from the list
	{
		currently_planted = false;
		plant_idx = -1;
	}
}

void hacks::setClanTag(IGameEvent* eevent) noexcept
{
	if (config::clanTag && !strcmp(eevent->GetName(), "item_pickup")) //event name from the list
	{
		if (interfaces::engine->IsConnected()) {
			memory::setClanTag("rollihook ~ ", "rollihook ~ ");
		}
	}
}

void sendBuylog(IGameEvent* eevent) noexcept {
	if (!config::buylogs)
		return;

	if (!strcmp(eevent->GetName(), "item_purchase")) {
		//interfaces::engine->GetUserFromID(eevent->GetInt("attacker")) == globals::localPlayerIdx

		if (config::ignore_buylogs_teammates && eevent->GetInt("team") == globals::localPlayer->GetTeam())
			return;

		PlayerInfo info;
		if (!interfaces::engine->GetPlayerInfo(eevent->GetInt("userid"), &info))
			return;
		
		std::string q = "echo [rollihook] ";
		q += info.name;
		q += " purchased ";
		q += eevent->GetString("weapon");
		interfaces::engine->cmd_unrestricted(q.c_str());
	}
}
void hacks::SendHitsound(IGameEvent* eevent) noexcept
{
	if (config::hitsound && !strcmp(eevent->GetName(), "player_hurt") && interfaces::engine->GetUserFromID(eevent->GetInt("attacker")) == globals::localPlayerIdx) //event name from the list
	{
		if (config::customHitSound)
		{
			interfaces::engine->cmd_unrestricted(("play " + config::customSoundName).c_str());
		}
		else
		{
			interfaces::engine->cmd_unrestricted("play buttons/arena_switch_press_02");
		}
	}

}

void hacks::RunAntiAim(CUserCmd* cmd) noexcept
{
	if (!config::anti_aim)
		return;

	if (!globals::localPlayer)
		return;

	if (!interfaces::engine->IsInGame())
		return;


	if (config::anti_aim_moonwalk) {
		cmd->buttons ^= CUserCmd::IN_FORWARD | CUserCmd::IN_BACK | CUserCmd::IN_MOVERIGHT | CUserCmd::IN_MOVELEFT;
	}


	if ( GetAsyncKeyState(0x45) || cmd->buttons & CUserCmd::IN_ATTACK) {}
	else {
		cmd->viewAngles.x = config::anti_aim_pitch; // -89 = up, 89 = down
		cmd->viewAngles.y = cmd->viewAngles.y - 179.f; // backwards
	}
}
