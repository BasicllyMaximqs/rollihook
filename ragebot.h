#pragma once
#include <vector>
#include <string>

class CUserCmd;

namespace hacks
{
	void RunRagebot(CUserCmd* cmd) noexcept;
	
	std::vector<int> hitboxes = {};

	// 0 - head
	// 1 - chest
	// 2 - stomach
	// 3 - pelvis
	// 4 - legs
	
}

