/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/
#pragma once
#include "LuaObject.h"
#include "EventSource.h"
#include "ImeInput.h"

class Sprite;

class HostWindow : 
	public LuaObject
{
protected:
	// 不允许在栈上创建对象 因为有delete this
	HostWindow(void);
	~HostWindow(void);

public:
	RTTI_DECLARATIONS(HostWindow, LuaObject);

	static void CreateInstance(HostWindow **ppObj) { *ppObj = new HostWindow;}

	// set rect
	static int SetRect(lua_State *L);

	// set name (class is fixed)
	static int SetText(lua_State *L);

	// set style
	static int SetStyle(lua_State *L);

	void Create(HWND parent);

	static int Create(lua_State *L);

	BEGIN_LUA_METHOD_MAP(HostWindow)
		LUA_METHOD_ENTRY(Create)
		LUA_METHOD_ENTRY(AttachSprite)
		LUA_METHOD_ENTRY(SetRect)
	END_LUA_METHOD_MAP()

	static void RegisterWndClass();

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	static int LuaHandleMessage(lua_State *L);

	void OnPaint( lua_State *L, HWND hwnd );

	HWND GetHWND();

	void AttachSprite(Sprite *sp);

	static int AttachSprite(lua_State *L);

	void OnImeInput(lua_State *L, LPCTSTR text);

	void SetImePosition( float x, float y );

	void SetFocusSprite(Sprite *sp);

	Sprite *GetFocusSprite();

	void SetCaretHeight(float);

	void ShowCaret();

	void HideCaret();

	void SetCapture(Sprite *sp);

	void ReleaseCapture();

	void TrackMouseLeave(Sprite *sp);

private:
	void OnMouseEvent(lua_State *L, UINT message, WPARAM wparam, LPARAM lparam);
	LRESULT OnImeEvent(lua_State *L, UINT message, WPARAM wparam, LPARAM lparam);
	void SetGdipMode(Gdiplus::Graphics &g);

private:
	DWORD m_style;
	DWORD m_exStyle;
	Gdiplus::RectF m_rect;
	HWND m_hwndParent;
	HWND m_hwnd;
	//EventSource m_eventSource;
	ImeInput m_ime;
	RECT m_rectComposition;
	int m_caretHeight;
	Sprite *m_sprite;
	Sprite *m_spFocus;
	Sprite *m_spCapture;
	Sprite *m_spHover;
	std::tr1::unordered_set<Sprite *> m_setTrackMouseLeave;
};
