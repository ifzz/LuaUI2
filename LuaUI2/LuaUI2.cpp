
/*
** LuaUI2.cpp : Defines the exported functions for the DLL application.
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/

#include "stdafx.h"
#include "atlstr.h"
#include "LuaUI2.h"
#include "HostWindow.h"
#include "Sprite.h"
#include "EditSprite.h"
#include "CanvasSprite.h"
#include "LuaHttp.h"
#include "WinApi.h"
#include "LuaBitmap.h"
#include "LuaTimer.h"

lua_State *g_L;
static Gdiplus::GdiplusStartupOutput gdipHook;

// 这个文件只存放关于GUI的东西

int l_PostQuitMessage(lua_State *L)
{
	int code = luaL_checkinteger(L, 1);
	::PostQuitMessage(code);
	return 0;
}

int l_CreateBitmapFromFile(lua_State *L)
{
	CStringW path = luaL_checkwstring(L, 1);
	LuaBitmap *p = LuaBitmap::FromFile(path);
	if (p->Get())
	{
		p->PushToLua(L); // TODO 这里还要研究一下
	}
	else
	{
		lua_pushnil(L);
	}
	p->Unref();
	return 1;
}

int l_CreateHostWindow(lua_State *L)
{
	HostWindow *p = NULL;
	HostWindow::CreateInstance(&p);
	p->PushToLua(L);
	p->Unref();
	return 1;
}

int l_CreateSprite(lua_State *L)
{
	Sprite *p = NULL;
	Sprite::CreateInstance(&p);
	p->PushToLua(L);
	p->Unref();
	return 1;
}

int l_CreateRectangleSprite(lua_State *L)
{
	RectangleSprite *p = NULL;
	RectangleSprite::CreateInstance(&p);
	p->PushToLua(L);
	p->Unref();
	return 1;
}

int l_CreateTextSprite(lua_State *L)
{
	TextSprite *p = NULL;
	TextSprite::CreateInstance(&p);
	p->PushToLua(L);
	p->Unref();
	return 1;
}

int l_CreateEditSprite(lua_State *L)
{
	EditSprite *p = NULL;
	EditSprite::CreateInstance(&p);
	p->PushToLua(L);
	p->Unref();
	return 1;
}

int l_CreateCanvasSprite(lua_State *L)
{
	CanvasSprite *p = NULL;
	CanvasSprite::CreateInstance(&p);
	p->PushToLua(L);
	p->Unref();
	return 1;
}

int l_RunMessageLoop( lua_State *L )
{
	LOG(<<".");
	ULONG_PTR token;
	gdipHook.NotificationHook(&token);
	MSG msg;
	BOOL bRet;
	while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
	{ 
		if (bRet == -1)
		{
			// handle the error and possibly exit
			LOG(<<"ERROR");
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			//lua_Hook h = lua_gethook(L);
			//int c = lua_gethookcount(L);
			//LOG(<<" hook:"<< (void*)h << " count:" << c << " msg:" << (void*) msg.message);
			//lua_sethook(L, NULL, 0, 0);
		}
	}
	gdipHook.NotificationUnhook(token);
	LOG(<<"WM_QUIT");
	return 0;
}

static const struct luaL_Reg cfunctions[] = {
	{"RunMessageLoop", l_RunMessageLoop},
	{"PostQuitMessage", l_PostQuitMessage},
	{"CreateHostWindow", l_CreateHostWindow},
	{"CreateSprite", l_CreateSprite},
	{"CreateRectangleSprite", l_CreateRectangleSprite},
	{"CreateTextSprite", l_CreateTextSprite},
	{"CreateEditSprite", l_CreateEditSprite},
	{"CreateCanvasSprite", l_CreateCanvasSprite},
	{"CreateBitmapFromFile", l_CreateBitmapFromFile},
	{NULL, NULL}
};


extern "C" LUAUI2_API int luaopen_luaui2(lua_State *L)
{
	LOG("luaopen_luaui2 -->");
	KObject::InitMagic();
	g_L = L;

	ULONG_PTR g_gdipToken;
	Gdiplus::GdiplusStartupInput input;
	input.SuppressBackgroundThread = TRUE;
	GdiplusStartup(&g_gdipToken, &input, &gdipHook);

	luaL_register(L, "luaui2", cfunctions);
	
	lua_getglobal(L, "luaui2"); // luaui2
	LuaHttp *http = new LuaHttp;
	http->PushToLua(L); // luaui2 LuaHttp
	http->Unref();
	lua_setfield(L, -2, "Http"); // luaui2

	LuaTimer *timer = new LuaTimer;
	timer->PushToLua(L); // luaui2 LuaTimer
	timer->Unref();
	lua_setfield(L, -2, "TimerManager"); // luaui2
	lua_pop(L, 1);

	HostWindow::RegisterWndClass();

	luaopen_winapi(L);

	LOG("luaopen_luaui2 <--");
	return 0;
}
