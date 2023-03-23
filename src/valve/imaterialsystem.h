#pragma once
#include "../util/memory.h"

#include "ckeyvalues.h"

class IMaterial
{
public:
	enum EMaterialVarFlag : std::int32_t
	{
		IGNOREZ = 1 << 15,
		WIREFRAME = 1 << 28
	};


	constexpr void SetMaterialVarFlag(const std::int32_t flag, const bool on) noexcept
	{
		memory::Call<void>(this, 29, flag, on);
	}

	constexpr void colorModulate(float r, float g, float b) noexcept
	{
	    memory::Call<void>(this, 28, r,g,b);
	}

	const char* GetName() noexcept
	{
		return memory::Call<const char*>(this, 0);
	}

	const char* GetTextureGroupName() noexcept
	{
		return memory::Call<const char*>(this, 1);
	}

	constexpr bool isErrorMaterial() {
		return memory::Call<bool>(this, 42);
	}

	constexpr int getReferenceCount() {
		return memory::Call<int>(this, 56);
	}

};

class IMaterialSystem
{
public:


	constexpr IMaterial* CreateMaterial(const char* name, CKeyValues* kv) noexcept
	{
		return memory::Call<IMaterial*>(this, 83, name, kv);
	}

	constexpr IMaterial* FindMaterial(const char* name) noexcept
	{
		return memory::Call<IMaterial*>(this, 84, name, nullptr, true, nullptr);
	}
	short FirstMaterial()
	{
		return memory::Call<short>(this, 86);
	}

	short NextMaterial(short handle)
	{
		return memory::Call<short>(this, 87, handle);
	}

	short InvalidMaterial()
	{
		return memory::Call<short>(this, 88);
	}

	IMaterial* GetMaterial(short handle)
	{
		return memory::Call<IMaterial*>(this, 89, handle);
	}
};
