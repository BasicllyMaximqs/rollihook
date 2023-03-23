#pragma once
#include "interfaces.h"
#include "../../config.h"
#include "../../gui.h"
#include "globals.h"
#include "../valve/cviewsetup.h"
#include "../valve/ivpanel.h"
#include "../valve/GameEvent.h"

enum ClientFrameStage_t {
	FRAME_UNDEFINED = -1,
	FRAME_START,
	FRAME_NET_UPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	FRAME_NET_UPDATE_END,
	FRAME_RENDER_START,
	FRAME_RENDER_END
};

namespace hooks
{
	// call once to emplace all hooks
	void Setup();


	void Destroy() noexcept;

	// bypass return address checks (thx osiris)
	using AllocKeyValuesMemoryFn = void* (__thiscall*)(void*, const std::int32_t) noexcept;
	inline AllocKeyValuesMemoryFn AllocKeyValuesMemoryOriginal = nullptr;
	void* __stdcall AllocKeyValuesMemory(const std::int32_t size) noexcept;

	// example CreateMove hook
	using CreateMoveFn = bool(__thiscall*)(IClientModeShared*, float, CUserCmd*) noexcept;
	inline CreateMoveFn CreateMoveOriginal = nullptr;
	bool __stdcall CreateMove(float frameTime, CUserCmd* cmd) noexcept;

	constexpr void* VirtualFunction(void* thisptr, size_t index) noexcept
	{
		return (*static_cast<void***>(thisptr))[index];
	}

	using EndSceneFn = long(__thiscall*)(void*, IDirect3DDevice9*) noexcept;
	inline EndSceneFn EndSceneOriginal = nullptr;
	long __stdcall EndScene(IDirect3DDevice9* device) noexcept;

	using ResetFn = HRESULT(__thiscall*)(void*, IDirect3DDevice9*, D3DPRESENT_PARAMETERS*) noexcept;
	inline ResetFn ResetOriginal = nullptr;
	HRESULT __stdcall Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept;


	using DrawModelFn = void(__thiscall*)(
		void*,
		void*,
		const CDrawModelInfo&,
		CMatrix3x4*,
		float*,
		float*,
		const CVector&,
		const std::int32_t
		) noexcept;

	inline DrawModelFn DrawModelOriginal = nullptr;
	void __stdcall DrawModel(
		void* results,
		const CDrawModelInfo& info,
		CMatrix3x4* bones,
		float* flexWeights,
		float* flexDelayedWeights,
		const CVector& modelOrigin,
		const std::int32_t flags
	) noexcept;

	using DoPostScreenEffectsFn = bool(__thiscall*)(int value);
	inline DoPostScreenEffectsFn DoPostScreenEffectsOriginal = nullptr;
	bool __stdcall DoPostScreenEffectsHook(int value) noexcept;

	using FireClientSideFn = void(__thiscall*)(void*, IGameEvent* eevent);
	inline FireClientSideFn FireEventClientSideOriginal = nullptr;
	void __stdcall FireEventClientSide(IGameEvent* eevent);

	using OverrideViewFn = void(__thiscall*)(CViewSetup* vsView) noexcept;
	inline OverrideViewFn OverrideViewOriginal = nullptr;
	void __stdcall OverrideView(CViewSetup* vsView) noexcept;

	using PaintTraverseFn = void(__thiscall*)(IVPanel*, std::uintptr_t, bool, bool);
	inline PaintTraverseFn PaintTraverseOriginal = nullptr;
	void __stdcall PaintTraverse(std::uintptr_t vguiPanel, bool forceRepaint, bool allowForce) noexcept;

	float __fastcall Hook_GetScreenAspectRatio(
		void* pEcx,
		void* pEdx,
		int32_t iWidth,
		int32_t iHeight
	) noexcept;

	using FrameStageNotifyFn = void(__thiscall*)(void*, ClientFrameStage_t);
	inline FrameStageNotifyFn FrameStageNotifyOriginal = nullptr;
	void __stdcall FrameStageNotify(ClientFrameStage_t stage) noexcept;

	using GetColorModulationFn = void(__thiscall*)( void*, void*, float* r, float* g, float* b);
	inline GetColorModulationFn GetColorModulationOriginal = nullptr;
	void __stdcall GetColorModulation(void* ecx, void* edx, float* r, float* g, float* b) noexcept;


}
