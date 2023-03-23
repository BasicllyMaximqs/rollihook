#include "aimbot.h"

#include "src/core/globals.h"
#include "src/core/interfaces.h"
#include "config.h"
#include "gui.h"
#include "src/hacks/misc.h"



void hacks::RunAimbot(CUserCmd* cmd) noexcept
{
	if (!config::aimbot)
		return;
	// check if we are trying to shoot
	if (!(cmd->buttons & CUserCmd::IN_ATTACK))
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

	CVector bestAngle{ };

	CVector localEyePosition;
	CTrace trace;
	CMatrix3x4 bones[128];


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
		
		if (!player->SetupBones(bones, 128, 256, interfaces::globals->currentTime))
			continue;

		// our eye position
		
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

		interfaces::engineTrace->TraceRay(
			CRay{ localEyePosition, bones[8].Origin() },
			MASK_SHOT,
			CTraceFilter{ globals::localPlayer },
			trace
		);

		if (!trace.entity || trace.fraction < 0.97f)
			continue;

		CVector enemyAngle
		{
			(bones[8].Origin() - localEyePosition).ToAngle() - (cmd->viewAngles + aimPunch)
		};

		if (const float fov = std::hypot(enemyAngle.x, enemyAngle.y); fov < config::bestFov)
		{
			bestAngle = enemyAngle;
		}
	}



	cmd->viewAngles = cmd->viewAngles + (config::legit_magnet ? bestAngle.Scale(config::legitbot_scaling) : bestAngle );

	if (!config::aimbot_quiet) {
		interfaces::engine->SetViewAngles(cmd->viewAngles);
	}
}

