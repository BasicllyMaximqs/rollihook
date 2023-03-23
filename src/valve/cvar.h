#pragma once
#include "../util/memory.h"
#include "convar.h"

struct ConVar;

class ICvar {
public:
	ConVar* findVar(const char* name) {
		return memory::Call<ConVar*>(this, 15, name);
	}
};

class conCommandBase
{
public:
	void* vmt;
	conCommandBase* next;
	bool registered;
	const char* name;
	const char* helpString;
	int flags;
	conCommandBase* conCommandBases;
	void* accessor;
};