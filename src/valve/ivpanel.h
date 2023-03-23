#pragma once
#include "../util/memory.h"

class IVPanel
{
public:
	constexpr const char* GetName(std::uint32_t panel) noexcept
	{
		return memory::Call<const char*>(this, 36, panel);
	}

	constexpr void enable_keyboard_input(unsigned int id, bool state) {
		return memory::Call<void>(this, 31, id, state);
	}
	constexpr void enable_mouse_input(unsigned int id, bool state) {
		return memory::Call<void>(this, 32, id, state);
	}

};
