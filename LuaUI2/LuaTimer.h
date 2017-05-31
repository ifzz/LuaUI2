#pragma once
#include "luaobject.h"

#include <unordered_map>
#include <list>

namespace cs {

// 定时器包装 TODO 支持c++使用
class LuaTimer :
	public LuaObject
{
public:
	LuaTimer(void);
	virtual ~LuaTimer(void);

	static int SetOnceTimer(lua_State *L);
	static int SetTimer(lua_State *L);
	static int ResetTimer(lua_State *L);
	static int KillTimer(lua_State *L);

	BEGIN_LUA_METHOD_MAP(LuaTimer)
		LUA_METHOD_ENTRY(SetOnceTimer)
		LUA_METHOD_ENTRY(SetTimer)
		LUA_METHOD_ENTRY(ResetTimer)
		LUA_METHOD_ENTRY(KillTimer)
	END_LUA_METHOD_MAP()

private:
	void RegisterClass();
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	void OnTimer(UINT id);
	int NewId();
	void DeleteTimer(int id);

	struct TimerData {
		TimerData():
			id(0),
			cb(0),
			once(false) {}
		int id;
		int cb;
		bool once;
	private:
		DISALLOW_COPY_AND_ASSIGN(TimerData);
	};

private:
	HWND m_hwnd;
	std::tr1::unordered_map<int, TimerData*> m_mapId2Callback;
	std::list<int> m_listRecycleId;
	int m_nextId;
	lua_State *m_L;
};

} // namespace cs
