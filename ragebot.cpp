#include "ragebot.h"

#include "src/core/globals.h"
#include "src/core/interfaces.h"
#include "config.h"
#include "gui.h"
#include "src/hacks/misc.h"
#include <format>
#include <iostream>



void hacks::RunRagebot(CUserCmd* cmd) noexcept
{
	// check if we are trying to shoot
	if (!config::ragebot)
		return;

	if (globals::localPlayer->IsDefusing())
		return;

	CEntity* activeWeapon = globals::localPlayer->GetActiveWeapon();

	if (!activeWeapon)
		return;

	const int weaponType = activeWeapon->GetWeaponType();

	switch (weaponType)
	{
		case CEntity::WEAPONTYPE_MACHINEGUN:
		case CEntity::WEAPONTYPE_RIFLE:
		case CEntity::WEAPONTYPE_SHOTGUN:
		case CEntity::WEAPONTYPE_SNIPER:
		case CEntity::WEAPONTYPE_PISTOL:
		{
			if (!activeWeapon->GetClip())
				return;

			if (weaponType == CEntity::WEAPONTYPE_SNIPER)
			{
				if (!globals::localPlayer->IsScoped())
					return;
			}

			break;
		}

		default:
			return;
	}


	for (int i = 1; i <= interfaces::globals->maxClients; ++i)
	{
		CEntity* player = interfaces::entityList->GetEntityFromIndex(i);

		if (!player)
			continue;

		if (player->IsDormant() || !player->IsAlive())
			continue;

		if (player->GetTeam() == globals::localPlayer->GetTeam())
			continue;

		if (player->HasGunGameImmunity())
			continue;

		// player's bone matrix
		CMatrix3x4 bones[128];
		if (!player->SetupBones(bones, 128, 256, interfaces::globals->currentTime))
			continue;

		// our eye position

		CVector localEyePosition;
		globals::localPlayer->GetEyePosition(localEyePosition);

		// our aim punch
		CVector aimPunch{ };

		switch (weaponType)
		{
		case CEntity::WEAPONTYPE_RIFLE:
		case CEntity::WEAPONTYPE_SUBMACHINEGUN:
		case CEntity::WEAPONTYPE_MACHINEGUN:
			globals::localPlayer->GetAimPunch(aimPunch);
		}

		for (size_t i = 0; i < hacks::hitboxes.size(); i++) {
			{
				CTrace trace;
				interfaces::engineTrace->TraceRay(
					CRay{ localEyePosition , bones[hacks::hitboxes[i]].Origin() },
					0x46004009,
					CTraceFilter{ globals::localPlayer },
					trace
				);

				//x0, y0, x1, y1

				if (trace.fraction < 0.97f)
					continue;
				if (!trace.entity)
					continue;
				if (!trace.entity->IsAlive() || trace.entity->GetTeam() == globals::localPlayer->GetTeam())
					continue;

				CVector enemyAngle
				{
					(bones[hacks::hitboxes[i]].Origin() - localEyePosition).ToAngle() - (cmd->viewAngles + aimPunch)
				};

				//std::string games = std::to_string(activeWeapon->getInaccuracy());

				//std::cout << (games.c_str()) << std::endl;

				//bestAngle = enemyAngle;
				if (config::autostop) {
					if (cmd->forwardMove != 0.f) {
						cmd->forwardMove = cmd->forwardMove / 2;
					}
					if (cmd->sideMove != 0.f) {
						cmd->sideMove = cmd->forwardMove / 2;
					}

				}



				cmd->viewAngles = cmd->viewAngles + enemyAngle;

				cmd->buttons |= CUserCmd::IN_ATTACK;

			}
		}
	}
}

