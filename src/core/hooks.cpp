#include "hooks.h"

#include <stdexcept>
#include <intrin.h>
#include <array>
#include <cstring>
#include <stdio.h>
#include <string.h>
#include <deque>

// include minhook for epic hookage
#include "../../ext/minhook/minhook.h"

#include "../core/globals.h"
#include "../../gui.h"
#include "../hacks/misc.h"
#include <thread>
#include <d3d9.h>
#include "../../aimbot.h"
#include <iostream>
#include <algorithm>
#include <format>
#include <map>

HFont hFont;

void hooks::Setup()
{
	MH_Initialize();


	MH_CreateHook(
		memory::Get(interfaces::keyValuesSystem, 1),
		&AllocKeyValuesMemory,
		reinterpret_cast<void**>(&AllocKeyValuesMemoryOriginal)
	);

	MH_CreateHook(
		memory::Get(interfaces::clientMode, 24),
		&CreateMove,
		reinterpret_cast<void**>(&CreateMoveOriginal)
	);

	MH_CreateHook(
		memory::Get(interfaces::studioRender, 29),
		&DrawModel,
		reinterpret_cast<void**>(&DrawModelOriginal)
	);

	MH_CreateHook(
		memory::Get(interfaces::client, 37),
		&FrameStageNotify,
		reinterpret_cast<void**>(&FrameStageNotifyOriginal)
	);

	MH_CreateHook(
		memory::Get(interfaces::engine, 101),
		&Hook_GetScreenAspectRatio,
		reinterpret_cast<void**>(NULL)
	);

	MH_CreateHook(
		memory::Get(interfaces::clientMode, 18),
		&OverrideView,
		reinterpret_cast<void**>(&OverrideViewOriginal)
	);

	MH_CreateHook(
		memory::Get(interfaces::panel, 41),
		&PaintTraverse,
		reinterpret_cast<void**>(&PaintTraverseOriginal)
	);

	MH_CreateHook(
		memory::Get(interfaces::game_ev_manager,9),
		&hooks::FireEventClientSide,
		reinterpret_cast<void**>(&FireEventClientSideOriginal)
	);

	if (MH_CreateHook(
		VirtualFunction(gui::device, 42),
		&EndScene,
		reinterpret_cast<void**>(&EndSceneOriginal)
	)) throw std::runtime_error("Unable to hook EndScene()");

	if (MH_CreateHook(
		VirtualFunction(gui::device, 16),
		&Reset,
		reinterpret_cast<void**>(&ResetOriginal)
	)) throw std::runtime_error("Unable to hook Reset()");

	if (MH_EnableHook(MH_ALL_HOOKS))
		throw std::runtime_error("Unable to enable hooks");


	Beep(250, 250);
	interfaces::engine->GetScreenSize(globals::scr_x, globals::scr_y);
	gui::DestroyDirectX();
}



void hooks::Destroy() noexcept
{
	// restore hooks
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);

	// uninit minhook
	MH_Uninitialize();
}

CVector ClampAngles(CVector angles) {

	while (angles.y > 180)
		angles.y -= 360;

	while (angles.y < -180)
		angles.y += 360;

	if (angles.x > 89.0f)
		angles.x = 89.0f;

	if (angles.x < -89.0f)
		angles.x = -89.0f;

	angles.z = 0;
	return angles;

}

wchar_t* to_wstring(const char* str)
{
	const auto size = strlen(str) + 1;

	auto* wc = new wchar_t[size];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, wc, size);

	return wc;

	delete[] wc; //Edit: Added delete
}

static wchar_t* charToWChar(const char* text)
{
	const size_t size = strlen(text) + 1;
	wchar_t* wText = new wchar_t[size];
#pragma warning(suppress : 4996)
	mbstowcs(wText, text, size);
	return wText;

	delete[] wText;
}

void __stdcall hooks::OverrideView(CViewSetup* vsView) noexcept
{

	if (!globals::localPlayer)
		return;
	
	if (config::override_viewmodel)
	{
			//too lazy
	}


	if (globals::localPlayer->IsScoped() && config::scope_fov_override)
	{
		vsView->fov = config::view_fov;
	}
	else
	{
		if (config::fov_changer)
			vsView->fov = config::view_fov;
	}

	OverrideViewOriginal(vsView);

	CVector vecAngles;
	interfaces::engine->GetViewAngles(vecAngles);

	interfaces::input->bCameraInThirdPerson = config::thirdperson;


	if (interfaces::input->bCameraInThirdPerson)
	{
		CVector clamp_angles = ClampAngles(CVector{ vecAngles.x, vecAngles.y, 60.f });
		clamp_angles.z = 250.f;
		interfaces::input->vecCameraOffset = clamp_angles;
		//vsView->origin = CVector(vsView->origin.x + config::view_x, vsView->origin.y + config::view_y, vsView->origin.z + config::view_z);
	}



}

void __stdcall hooks::FireEventClientSide(IGameEvent* eevent)
{			
	if (interfaces::engine->IsInGame() && eevent) {
		hacks::bombRevealPlant(eevent);
		hacks::setClanTag(eevent);
		hacks::KillSay(eevent);
		hacks::SendHitsound(eevent);
	}

    return FireEventClientSideOriginal(interfaces::game_ev_manager, eevent);
}

void __stdcall hooks::FrameStageNotify( ClientFrameStage_t stage) noexcept
{
	if (!globals::localPlayer)
		return FrameStageNotifyOriginal(interfaces::client, stage);

	hacks::removeVisualRecoil(stage);

	if (config::no_flash && globals::localPlayer->isFlashed())
	{
		globals::localPlayer->m_flFlashMaxAlpha() = 0.f;
	}
	else
	{
		globals::localPlayer->m_flFlashMaxAlpha() = 255.f;
	}

	auto post_processing = interfaces::cvar->findVar("mat_postprocess_enable");

	post_processing->SetValue(config::disable_post_processing ? 0 : 1);

	auto m_c_r = interfaces::cvar->findVar("mat_ambient_light_r");
	auto m_c_g = interfaces::cvar->findVar("mat_ambient_light_g");
	auto m_c_b = interfaces::cvar->findVar("mat_ambient_light_b");

	if (config::map_cst_colors) {
		m_c_r->SetValue(config::map_coloring[0]);
		m_c_g->SetValue(config::map_coloring[1]);
		m_c_b->SetValue(config::map_coloring[2]);
	}
	else {
		m_c_r->SetValue(0);
		m_c_g->SetValue(0);
		m_c_b->SetValue(0);
	}

	static auto sky = interfaces::cvar->findVar("r_3dsky");
	sky->SetValue(config::remove_3dsky);

	auto shadows = interfaces::cvar->findVar("cl_csm_enabled");
	shadows->SetValue(config::remove_shadows);

	FrameStageNotifyOriginal(interfaces::client, stage);
}

void* __stdcall hooks::AllocKeyValuesMemory(const std::int32_t size) noexcept
{

	if (const std::uint32_t address = reinterpret_cast<std::uint32_t>(_ReturnAddress());
		address == reinterpret_cast<std::uint32_t>(memory::allocKeyValuesEngine) ||
		address == reinterpret_cast<std::uint32_t>(memory::allocKeyValuesClient)) 
		return nullptr;


	return AllocKeyValuesMemoryOriginal(interfaces::keyValuesSystem, size);
}

bool __stdcall hooks::CreateMove(float frameTime, CUserCmd* cmd) noexcept
{
	CreateMoveOriginal(interfaces::clientMode, frameTime, cmd);

	if (!cmd || !cmd->commandNumber)
		return CreateMoveOriginal(interfaces::clientMode, frameTime, cmd);

	if (CreateMoveOriginal(interfaces::clientMode, frameTime, cmd))
		interfaces::engine->SetViewAngles(cmd->viewAngles);

	globals::UpdateLocalPlayer();

	if (globals::localPlayer && globals::localPlayer->IsAlive())
	{
		// example bhop		
		hacks::RunBunnyHop(cmd);
		hacks::RunAntiAim(cmd);
		hacks::RunAimbot(cmd);
		hacks::RunRagebot(cmd);
	}


	if (config::anti_aim && globals::localPlayer->IsAlive()) //override here cus kp
	{
		if (!(cmd->buttons & CUserCmd::IN_USE) && !(cmd->buttons & CUserCmd::IN_ATTACK)) // E (use) OR attack by CUserCmd
		{
			if (GetAsyncKeyState(0x57)) //W
			{
				cmd->forwardMove = -450.f;
			}
			if (GetAsyncKeyState(0x41)) // a
			{
				cmd->sideMove = 450.f;
			}
			if (GetAsyncKeyState(0x53)) // s
			{
				cmd->forwardMove = 450.f;
			}
			if (GetAsyncKeyState(0x44)) // d
			{
				cmd->sideMove = -450.f;
			}
		}


	}

	cmd->viewAngles = ClampAngles(CVector(cmd->viewAngles.x, cmd->viewAngles.y, 0));

	
	return globals::localPlayer->IsAlive() ? false : CreateMoveOriginal(interfaces::clientMode, frameTime, cmd);
}

float __fastcall hooks::Hook_GetScreenAspectRatio(
	void* pEcx,
	void* pEdx,
	int32_t iWidth,
	int32_t iHeight
) noexcept
{
	return config::aspectRatio ? config::aspectRatioAngle : 1.88f;
}

long __stdcall hooks::EndScene(IDirect3DDevice9* device) noexcept
{
	static const auto returnAddress = _ReturnAddress();

	const auto result = EndSceneOriginal(device, device);

	// stop endscene getting called twice
	if (_ReturnAddress() == returnAddress)
		return result;

	if (!gui::setup)
		gui::SetupMenu(device);

	if (gui::open)
		gui::Render();


	return result;
}

HRESULT __stdcall hooks::Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	const auto result = ResetOriginal(device, device, params);
	ImGui_ImplDX9_CreateDeviceObjects();
	return result;
}

void __stdcall hooks::DrawModel(
	void* results,
	const CDrawModelInfo& info,
	CMatrix3x4* bones,
	float* flexWeights,
	float* flexDelayedWeights,
	const CVector& modelOrigin,
	const std::int32_t flags
) noexcept
{
	//main menu render fix, just return the func lol
	if (!globals::localPlayer)
		return DrawModelOriginal(interfaces::studioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);;

	if (info.renderable) {
		CEntity* entity = info.renderable->GetIClientUnknown()->GetBaseEntity();

		if (entity && entity->IsPlayer())
		{
			if (config::chams)
			{
				if ((entity->GetTeam() != globals::localPlayer->GetTeam() && config::chams_drawenemies == false) 
					|| (entity->GetTeam() == globals::localPlayer->GetTeam() && config::chams_drawfriendlies == false))
					return DrawModelOriginal(interfaces::studioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);

				static IMaterial* material = interfaces::materialSystem->FindMaterial("debug/debugambientcube");

				interfaces::studioRender->SetAlphaModulation(1.0f);

				material->SetMaterialVarFlag(IMaterial::IGNOREZ, true);
				if(entity->GetTeam() != globals::localPlayer->GetTeam())
					interfaces::studioRender->SetColorModulation(config::chams_occluded);
				else
					interfaces::studioRender->SetColorModulation(config::chams_friendly_occluded);
				interfaces::studioRender->ForcedMaterialOverride(material);
				DrawModelOriginal(interfaces::studioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);



				material->SetMaterialVarFlag(IMaterial::IGNOREZ, false);
				if (entity->GetTeam() != globals::localPlayer->GetTeam())
					interfaces::studioRender->SetColorModulation(config::chams_visible);
				else
					interfaces::studioRender->SetColorModulation(config::chams_friendly_visible);
				interfaces::studioRender->ForcedMaterialOverride(material);
				DrawModelOriginal(interfaces::studioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);

				return interfaces::studioRender->ForcedMaterialOverride(nullptr);
			}
		}
	}

	DrawModelOriginal(interfaces::studioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);
}

static bool WorldToScreen(const CVector& point, CVector& screen) noexcept
{
	// get the w2s matrix
	const CMatrix4x4& matrix = interfaces::engine->WorldToScreenMatrix();

	// calc width first to test whether on screen or not
	float w = matrix.data[3][0] * point.x + matrix.data[3][1] * point.y + matrix.data[3][2] * point.z + matrix.data[3][3];

	// not on screen
	if (w < 0.001f)
		return false;

	float inverse = 1.f / w;

	screen.x = (matrix.data[0][0] * point.x + matrix.data[0][1] * point.y + matrix.data[0][2] * point.z + matrix.data[0][3]) * inverse;
	screen.y = (matrix.data[1][0] * point.x + matrix.data[1][1] * point.y + matrix.data[1][2] * point.z + matrix.data[1][3]) * inverse;

	int x, y;
	interfaces::engine->GetScreenSize(x, y);

	screen.x = (x * 0.5f) + (screen.x * x) * 0.5f;
	screen.y = (y * 0.5f) - (screen.y * y) * 0.5f;

	// on screen
	return true;
}


void __stdcall hooks::PaintTraverse(std::uintptr_t vguiPanel, bool forceRepaint, bool allowForce) noexcept
{
	hacks::RunNightMode();

	if (vguiPanel == interfaces::engineVGui->GetPanel(PANEL_TOOLS))
	{
		// make sure we are in-game
		if (interfaces::engine->IsInGame() && globals::localPlayer)
		{
			// loop through players
			for (int i = 1; i <= interfaces::globals->maxClients; ++i)
			{
				// get the player
				CEntity* player = interfaces::entityList->GetEntityFromIndex(i);

				// make sure player is valid
				if (!player)
					continue;


				// make sure they aren't dormant && are alive
				if (player->IsDormant() || !player->IsAlive())
					continue;

				// no esp on teammates
				if (player->GetTeam() == globals::localPlayer->GetTeam())
					continue;

				// dont do esp on who we are spectating
				if (!globals::localPlayer->IsAlive())
					if (globals::localPlayer->GetObserverTarget() == player)
						continue;



				// player's bone matrix
				CMatrix3x4 bones[128];
				if (!player->SetupBones(bones, 128, 0x7FF00, interfaces::globals->currentTime))
					continue;


				// screen position of head
				// we add 11.f here because we want the box ABOVE their head
				CVector top;
				if (!WorldToScreen(bones[8].Origin() + CVector{0.f, 0.f, 9.f}, top))
				{
					continue;
				}

				// screen position of feet
				// we subtract 9.f here because we want the box BELOW their feet
				CVector bottom;
				if (!WorldToScreen(player->GetAbsOrigin() - CVector{ 0.f, 0.f, 9.f }, bottom))
					continue;

				// the height of the box is the difference between
				// the bottom (larger number) and the top
				const float h = bottom.y - top.y;

				// we can use the height to determine a width
				const float w = h * 0.3f;

				const auto left = static_cast<int>(top.x - w);
				const auto right = static_cast<int>(top.x + w);
				
				interfaces::surface->DrawSetColor(255,255,255, 255);

				if (config::esp_snapline) {
					interfaces::surface->DrawSetColor(config::esp_snap[0] * 255, config::esp_snap[1] * 255, config::esp_snap[2] * 255, 255);
					interfaces::surface->DrawLine(globals::scr_x / 2, globals::scr_y / 2, top.x, top.y);
				}


					//int w, h;

				PlayerInfo pInfo;

				if (!interfaces::engine->GetPlayerInfo(player->GetIndex(), &pInfo))
					continue;

				if (!hFont)
				{
					hFont = interfaces::surface->FontCreate();
					//0x010 = FONTFLAG_ANTIALIAS
					//0x200 = FONTFLAG_OUTLINE
					interfaces::surface->SetFontGlyphSet(hFont, "Terminal", 14, 600, 0, 0, 0x200 | 0x010);

				}

				interfaces::surface->DrawSetTextFont(hFont);
				interfaces::surface->DrawSetTextColor(255, 255, 255, 255);
				if (config::esp_name)
				{
					const wchar_t* gamesense = charToWChar(pInfo.name);

					//int width_q, height_q;
					CVector xv = interfaces::surface->GetTextSize(hFont, gamesense);
					interfaces::surface->DrawSetTextPos(top.x - (xv.x / 2), top.y - 15);
					interfaces::surface->DrawPrintText(gamesense, wcslen(gamesense), FONT_DRAW_DEFAULT);
					
					
				}
				
				if (config::esp_weapon) {
					CEntity* activeWeapon = player->GetActiveWeapon();
				}

				if (config::esp_skeleton) {
					// ids:
					// 7 to 8

					// 7 to 41 to 42 to 43
					// 7 to 11 to 12 to 13

					// 7 to 0

					// 0 to 78 to 79
					// 0 to 71 to 72
					// well this is fucking unoptimized x)
					CVector head;
					CVector upperChest;
					CVector rightArm1;
					CVector rightArm2;
					CVector rightArm3;
					CVector leftArm1;
					CVector leftArm2;
					CVector leftArm3;
					CVector pelvis;
					CVector rightLeg1;
					CVector rightLeg2;
					CVector leftLeg1;
					CVector leftLeg2;
					if(!WorldToScreen(bones[8].Origin(), head)) continue;
					if(!WorldToScreen(bones[7].Origin(), upperChest)) continue;
					if(!WorldToScreen(bones[41].Origin(), rightArm1)) continue;
					if(!WorldToScreen(bones[42].Origin(), rightArm2)) continue; 
					if(!WorldToScreen(bones[43].Origin(), rightArm3)) continue;
					if(!WorldToScreen(bones[11].Origin(), leftArm1)) continue;
					if(!WorldToScreen(bones[12].Origin(), leftArm2)) continue;
					if(!WorldToScreen(bones[13].Origin(), leftArm3)) continue;
					if(!WorldToScreen(bones[0].Origin(), pelvis)) continue;
					if(!WorldToScreen(bones[78].Origin(), rightLeg1)) continue; 
					if(!WorldToScreen(bones[79].Origin(), rightLeg2)) continue; 
					if(!WorldToScreen(bones[71].Origin(), leftLeg1)) continue; 
					if(!WorldToScreen(bones[72].Origin(), leftLeg2)) continue; 
					interfaces::surface->DrawSetColor(config::esp_skel[0], config::esp_skel[1], config::esp_skel[2]);

					interfaces::surface->DrawLine(head, upperChest);
					interfaces::surface->DrawLine(upperChest, rightArm1);
					interfaces::surface->DrawLine(upperChest, leftArm1);

					interfaces::surface->DrawLine(rightArm1, rightArm2);
					interfaces::surface->DrawLine(rightArm2, rightArm3);

					interfaces::surface->DrawLine(leftArm1, leftArm2);
					interfaces::surface->DrawLine(leftArm2, leftArm3);

					interfaces::surface->DrawLine(upperChest, pelvis);

					interfaces::surface->DrawLine(pelvis, rightLeg1);
					interfaces::surface->DrawLine(rightLeg1, rightLeg2);

					interfaces::surface->DrawLine(pelvis, leftLeg1);
					interfaces::surface->DrawLine(leftLeg1, leftLeg2);
				}


				int flag_columns = 0; // flagstack

				//check regions at side
				if (config::esp_flashed && player->isFlashed()) {
					
					interfaces::surface->DrawSetTextPos(right, top.y + 15 * flag_columns);

					std::wstring gamesense = gui::s2ws("Flashed");
					interfaces::surface->DrawPrintText(gamesense, gamesense.length(), FONT_DRAW_DEFAULT);
					flag_columns += 1;
				}

				if (config::esp_defuser && player->hasDefuser())
				{
					interfaces::surface->DrawSetTextPos(right, top.y + 15 * flag_columns);

					std::wstring gamesense = gui::s2ws("Defuser");
					interfaces::surface->DrawPrintText(gamesense, gamesense.length(), FONT_DRAW_DEFAULT);
					flag_columns += 1;
				}

				if (config::esp_display_hp_text) {

					interfaces::surface->DrawSetTextPos(right, top.y + 15 * flag_columns);

					std::wstring assembly = std::to_wstring(player->GetHealth());
					assembly += L"HP";

					interfaces::surface->DrawPrintText(assembly, assembly.length(), FONT_DRAW_DEFAULT);
					flag_columns += 1;
				}

				if (config::esp_show_if_scoped && player->IsScoped())
				{
					interfaces::surface->DrawSetTextPos(right, top.y + 15 * flag_columns);

					std::wstring gamesense = gui::s2ws("Scoped");
					interfaces::surface->DrawPrintText(gamesense, gamesense.length(), FONT_DRAW_DEFAULT);
					flag_columns += 1;
				}

				if (config::esp)
				{
				// set the color to white

					int n1 = static_cast<int>(config::esp_box[0] * 255);
					int n2 = static_cast<int>(config::esp_box[1] * 255);
					int n3 = static_cast<int>(config::esp_box[2] * 255);

					interfaces::surface->DrawSetColor(n1, n2, n3, 255);

					// draw the normal box
					interfaces::surface->DrawOutlinedRect(left, top.y, right, bottom.y);

					interfaces::surface->DrawSetColor(0, 0, 0, 255);

					// normal box outline
					interfaces::surface->DrawOutlinedRect(left - 1, top.y - 1, right + 1, bottom.y + 1);
					interfaces::surface->DrawOutlinedRect(left + 1, top.y + 1, right - 1, bottom.y - 1);
				}
				if (config::esp_healthbar)
				{
					interfaces::surface->DrawSetColor(1,1,1, 255);
					interfaces::surface->DrawOutlinedRect(left - 6, top.y - 1, left - 3, bottom.y + 1);


					const float healthFrac = player->GetHealth() * 0.01f;

					interfaces::surface->DrawSetColor((1.f - healthFrac) * 255, 255 * healthFrac, 0, 255);

					interfaces::surface->DrawFilledRect(left - 5, bottom.y - (h * healthFrac), left - 4, bottom.y);
				}
				
			}

			if (config::reveal_plant && hacks::currently_planted)
			{
				interfaces::surface->DrawSetColor(255, 255, 255, 255);
				interfaces::surface->DrawSetTextPos(globals::scr_x, globals::scr_y /2);
				
				std::wstring pre = gui::s2ws(std::to_string(hacks::plant_idx));

				std::wstring crack = L"Bombsite - ";
				crack += pre;
				interfaces::surface->DrawPrintText(crack, crack.length(), FONT_DRAW_ADDITIVE);
			}
		}
	}

	if (config::remove_blackscope && globals::localPlayer->IsScoped() && (!strcmp("HudZoom", interfaces::panel->GetName(vguiPanel))))
	{
		CEntity* activeWeapon = globals::localPlayer->GetActiveWeapon();

		if (!activeWeapon)
			return;

		const int weaponType = activeWeapon->GetWeaponType();

		if (weaponType == CEntity::WEAPONTYPE_SNIPER) {
			interfaces::surface->DrawSetColor(1, 1, 1, 255);
			interfaces::surface->DrawLine(0, globals::scr_y / 2, globals::scr_x, globals::scr_y / 2);
			interfaces::surface->DrawLine(globals::scr_x / 2, 0, globals::scr_x / 2, globals::scr_y);
		}
		return;
	}

	// call original function
	return PaintTraverseOriginal(interfaces::panel, vguiPanel, forceRepaint, allowForce);
}
