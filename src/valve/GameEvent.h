#pragma once

class IGameEvent {
public:
	const char* GetName() {
		return memory::Call<const char*>(this, 1);
		//return Util::GetVFunc<const char* (__thiscall*)(void*)>(this, 1)(this);
	}

	int GetInt(const char* szKeyName, int nDefault = 0) {
		return memory::Call<int>(this, 6, szKeyName, nDefault);
	}

	const char* GetString(const char* szKeyName) {
		return memory::Call<const char*>(this, 6, szKeyName);
	}

};

class IGameEventManager2 {
public:
	bool FireEventClientSide(IGameEvent* pEvent) {
		return memory::Call<bool>(this, 8, pEvent);
	}
};
