/*
** WinApi.cpp : Bind windows api to lua
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/

#include "StdAfx.h"
#include "Common.h"
#include "WinApi.h"

// 各种面向过程的Windows API绑定

#define LUA_ROUTINE(x) static int x(lua_State *L)

static int l_GetUtf16Table(lua_State *L)
{
	CStringW str = luaL_checkwstring(L, 1);
	lua_newtable(L);
	for (int i = 0; i < str.GetLength(); i ++)
	{
		lua_pushinteger(L, str.GetAt(i));
		lua_rawseti(L, -2, i + 1);
	}
	return 1;
}

static int l_GetLogicalDrivers(lua_State *L)
{
	DWORD drivers = ::GetLogicalDrives();
	int i = 0;
	CStringA strDrive;
	lua_newtable(L);
	int idx = 1;
	while (i < 32)
	{
		if (drivers & 1)
		{
			strDrive.Format("%c:\\", 'A' + i);
			lua_pushinteger(L, idx);
			lua_pushstring(L, strDrive);
			lua_settable(L, -3);
			idx++;
			//CsDebugA("l_get_logical_drivers() %s\r\n", strDrive);
		}
		drivers = drivers >> 1;
		i++;
	}
	return 1;
}

static int l_SafeWriteFile(lua_State *L)
{
	return 0;
}

static int l_PathFileExists(lua_State *L)
{
	CStringW path = luaL_checkwstring(L, 1);
	BOOL ret = ::PathFileExists(path);
	lua_pushboolean(L, ret);
	return 1;
}

static int l_Trace(lua_State *L)
{
	CStringW log(L"<LUA> ");
	for (int i = 1; i <= lua_gettop(L); i ++)
	{
		switch(lua_type(L, i))
		{
		case LUA_TNUMBER:
			log.AppendFormat(L"%.2f ", lua_tonumber(L, i));
			break;
		case LUA_TSTRING:
			log.AppendFormat(L"%s ", luaL_checkwstring(L, i));
			break;
		default:
			log.AppendFormat(L"(%S:%p) ", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
		}
	}
	log.Append(L"\r\n");
	::OutputDebugStringW(log);
	return 0;
}

static int l_SetCursor(lua_State *L)
{
	int id = luaL_checkinteger(L, 1);
	// IDC_ARROW
	::SetCursor(::LoadCursor(NULL, reinterpret_cast<LPCTSTR>(id)));
	return 0;
}

static int l_GetModuleFileName(lua_State *L)
{
	int hMod = 0;
	if (lua_gettop(L) > 1)
	{
		hMod = luaL_checkinteger(L, 1);
	}
	wchar_t path[MAX_PATH];
	::GetModuleFileNameW((HMODULE)hMod, path, MAX_PATH);
	luaL_pushwstring(L, path);
	//CStringA str = Utf16ToUtf8(path, wcslen(path));
	//lua_pushstring(L, str);
	return 1;
}

static int l_SHGetFolderPath(lua_State *L)
{
	int folder = CSIDL_APPDATA;
	if (lua_gettop(L) > 1)
	{
		folder = luaL_checkinteger(L, 1);
	}
	wchar_t path[MAX_PATH];
	if (SUCCEEDED(::SHGetFolderPath(NULL, folder, NULL, SHGFP_TYPE_CURRENT, path)))
	{
		luaL_pushwstring(L, path);
		return 1;
	}
	return 0;
}

static const struct luaL_Reg cfunctions[] = {
	{"GetUtf16Table", l_GetUtf16Table},
	{"GetLogicalDrivers", l_GetLogicalDrivers},
	{"PathFileExists", l_PathFileExists},
	{"SafeWriteFile", l_SafeWriteFile},
	{"Trace", l_Trace},
	{"SetCursor", l_SetCursor},
	{"GetModuleFileName", l_GetModuleFileName},
	{"SHGetFolderPath", l_SHGetFolderPath},
	{ NULL, NULL }
};

int luaopen_winapi( lua_State *L )
{
	luaL_register(L, "winapi", cfunctions);
	return 0;
}
