/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/
#pragma once
#include "LuaObject.h"
#include "EventSource.h"
#include "ImeInput.h"

namespace cs {

class Sprite;

class IMessageHandler
{
    LRESULT HandleMessage(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, bool &bHandled);
};

class HostWindowLua;

class HostWindow : 
	public Object
{
protected:
	virtual ~HostWindow(void); // 保护的析构函数 足以禁止在栈上创建对象
public:
    HostWindow(void);

    void SetRect(Gdiplus::RectF rc);

	static int SetText(lua_State *L);

	static int SetStyle(lua_State *L);

    void Create(HostWindow *parent, Gdiplus::RectF rc, DWORD style, DWORD exStyle);

	static void RegisterWndClass();

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	static int LuaHandleMessage(lua_State *L);

	void OnPaint( HWND hwnd );

	HWND GetHWND();

	void AttachSprite(Sprite *sp);

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

    virtual LuaObject * GetLuaSide() override;

    enum Event
    {
        eHostFirst,
        eWin32Message,
        eHostLast
    };

private:
	void HandleMouseMessage(lua_State *L, UINT message, WPARAM wparam, LPARAM lparam);
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
    HostWindowLua *m_luaSide;
};

} // namespace cs