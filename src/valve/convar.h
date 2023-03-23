#pragma once
#include "../util/memory.h"
#include <type_traits>
#include <vector>

template <typename T>
class UtlVector {
public:
	constexpr T& operator[](int i) noexcept { return memory[i]; }
	constexpr const T& operator[](int i) const noexcept { return memory[i]; }

	T* memory;
	int allocationCount;
	int growSize;
	int size;
	T* elements;
};

enum cvar_flags {
	fcvar_none = 0,
	fcvar_unregistered = (1 << 0),
	fcvar_developmentonly = (1 << 1),
	fcvar_gamedll = (1 << 2),
	fcvar_clientdll = (1 << 3),
	fcvar_hidden = (1 << 4),
	fcvar_protected = (1 << 5),
	fcvar_sponly = (1 << 6),
	fcvar_archive = (1 << 7),
	fcvar_notify = (1 << 8),
	fcvar_userinfo = (1 << 9),
	fcvar_printableonly = (1 << 10),
	fcvar_unlogged = (1 << 11),
	fcvar_never_as_string = (1 << 12),
	fcvar_replicated = (1 << 13),
	fcvar_cheat = (1 << 14),
	fcvar_ss = (1 << 15),
	fcvar_demo = (1 << 16),
	fcvar_dontrecord = (1 << 17),
	fcvar_ss_added = (1 << 18),
	fcvar_release = (1 << 19),
	fcvar_reload_materials = (1 << 20),
	fcvar_reload_textures = (1 << 21),
	fcvar_not_connected = (1 << 22),
	fcvar_material_system_thread = (1 << 23),
	fcvar_archive_xbox = (1 << 24),
	fcvar_accessible_from_threads = (1 << 25),
	fcvar_server_can_execute = (1 << 28),
	fcvar_server_cannot_query = (1 << 29),
	fcvar_clientcmd_can_execute = (1 << 30),
	fcvar_unused = (1 << 31),
	fcvar_material_thread_mask = (fcvar_reload_materials | fcvar_reload_textures | fcvar_material_system_thread)
};

struct ConVar {
public:

	float getFloat() {
		return memory::Call<float>(this, 12);
	}

	int getInt() {
		return memory::Call<int>(this, 13);
	}

	void SetValue(const char* value) {
		memory::Call<const char*>(this, 14, value);
	}

	void SetValue(float value) {
		memory::Call<float>(this, 15, value);
	}

	void SetValue(int value) {
		memory::Call<int>(this, 16, value);
	}

	std::byte pad[24];
	std::add_pointer_t<void __cdecl()> changeCallback;
	ConVar* parent;
	const char* defaultValue;
	char* string;
	std::byte pad1[28];
	UtlVector<void(__cdecl*)()> onChangeCallbacks;
};
