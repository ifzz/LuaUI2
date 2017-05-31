#include "StdAfx.h"
#include "LuaTimer.h"

extern HINSTANCE g_hInstance;

namespace cs {

LuaTimer::LuaTimer(void) :
	m_hwnd(NULL),
	m_nextId(0),
	m_L(NULL)
{
	RegisterClass(); // 好像这个可以多次调用而无副作用
	m_hwnd = ::CreateWindowEx( 0, L"LuaTimer", L"LuaTimer", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, g_hInstance, this );
}

LuaTimer::~LuaTimer(void)
{
	::DestroyWindow(m_hwnd);
}

void LuaTimer::RegisterClass()
{
	WNDCLASS wc; 
	// Register the main window class. 
	wc.style = CS_HREDRAW | CS_VREDRAW; 
	wc.lpfnWndProc = (WNDPROC) WndProc; 
	wc.cbClsExtra = 0; 
	wc.cbWndExtra = 0; 
	wc.hInstance = g_hInstance; 
	wc.hIcon = NULL; // LoadIcon(NULL, IDI_APPLICATION); 
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); 
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); 
	wc.lpszMenuName =  NULL; 
	wc.lpszClassName = L"LuaTimer"; 

	if (!::RegisterClass(&wc)) 
	{	assert(false); }
}

LRESULT CALLBACK LuaTimer::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_NCCREATE:
		do 
		{
			CREATESTRUCT *pcs = (CREATESTRUCT *)lparam;
			::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pcs->lpCreateParams);
		} while (0);
		break;
	case WM_TIMER:
		do 
		{
			LuaTimer *thiz = (LuaTimer *)::GetWindowLongPtr(hwnd, GWLP_USERDATA);
			thiz->OnTimer(wparam);
		} while (0);
		break;
	}
	return ::DefWindowProc(hwnd, msg, wparam, lparam);
}

int LuaTimer::SetOnceTimer( lua_State *L )
{
	LuaTimer *thiz = CheckLuaObject<LuaTimer>(L, 1);
	thiz->m_L = L;
	int interval = luaL_checkinteger(L, 2);
	luaL_checktype(L, 3, LUA_TFUNCTION);
	lua_pushvalue(L, 3);
	int cb = luaL_ref(L, LUA_REGISTRYINDEX);
	TimerData *pData = new TimerData;
	pData->id = thiz->NewId();
	pData->cb = cb;
	pData->once = true;
	thiz->m_mapId2Callback[pData->id] = pData;
	assert(::IsWindow(thiz->m_hwnd));
	::SetTimer(thiz->m_hwnd, pData->id, interval, NULL);
	return 0;
}

int LuaTimer::SetTimer( lua_State *L )
{
	LuaTimer *thiz = CheckLuaObject<LuaTimer>(L, 1);
	thiz->m_L = L;
	int interval = luaL_checkinteger(L, 2);
	luaL_checktype(L, 3, LUA_TFUNCTION);
	lua_pushvalue(L, 3);
	int cb = luaL_ref(L, LUA_REGISTRYINDEX);
	TimerData *pData = new TimerData;
	pData->id = thiz->NewId();
	pData->cb = cb;
	pData->once = false;
	thiz->m_mapId2Callback[pData->id] = pData;
	assert(::IsWindow(thiz->m_hwnd));
	::SetTimer(thiz->m_hwnd, pData->id, interval, NULL);

	lua_pushinteger(L, pData->id);
	return 1;
}

int LuaTimer::ResetTimer( lua_State *L )
{
	// TODO 这个还要测
	LuaTimer *thiz = CheckLuaObject<LuaTimer>(L, 1);
	thiz->m_L = L;
	int id = luaL_checkinteger(L, 2);
	int interval = luaL_checkinteger(L, 3);
	::SetTimer(thiz->m_hwnd, id, interval, NULL);
	return 0;
}

int LuaTimer::KillTimer( lua_State *L )
{
	LuaTimer *thiz = CheckLuaObject<LuaTimer>(L, 1);
	thiz->m_L = L;
	int id = luaL_checkinteger(L, 2);
	thiz->DeleteTimer(id);
	return 0;
}

void LuaTimer::OnTimer( UINT id )
{
	TimerData *pData = m_mapId2Callback[id];
	assert(pData);
	lua_State *L = m_L;
	lua_rawgeti(L, LUA_REGISTRYINDEX, pData->cb);
	lua_pushinteger(L, id);
	LuaPCall(L, 1, 0);
	if (pData->once)
	{
		DeleteTimer(id);
	}
}

int LuaTimer::NewId()
{
	if (m_listRecycleId.empty())
	{
		m_nextId ++;
		assert(m_nextId > 0); // overflow
		return m_nextId;
	}
	else
	{
		std::list<int>::iterator iter = m_listRecycleId.begin();
		int id = *iter;
		m_listRecycleId.pop_front();
		return id;
	}
}

void LuaTimer::DeleteTimer( int id )
{
	LOG("id:"<<id);
	lua_State *L = m_L;
	TimerData *pData = m_mapId2Callback[id];
	if (!pData)
	{
		LOG("pData NULL"); // FIXME 这里为什么会多进来一次 明明已经KillTimer了
		return;
	}
	BOOL ret = ::KillTimer(m_hwnd, id);
	assert(ret);

	std::tr1::unordered_map<int, TimerData*>::iterator iter = m_mapId2Callback.find(id);
	assert(iter != m_mapId2Callback.end());
	m_mapId2Callback.erase(iter);

	luaL_unref(L, LUA_REGISTRYINDEX, pData->cb);

	m_listRecycleId.push_back(id);

	delete pData;
}

} // namespace cs
