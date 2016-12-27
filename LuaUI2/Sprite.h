/*
** Sprite.h The core element of LuaUI2
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/
#pragma once
#include "LuaObject.h"
#include "HostWindow.h"

struct MouseEvent
{
	UINT message;
	float x;  // TODO 改成float GDI+里面太多float了 导致转来转去
	float y;
	UINT flag; // ctrl shift atl
	float delta; // wheel
};

class Sprite : public LuaObject
{
protected:
	Sprite(void);
	virtual ~Sprite(void);

public:
	static void CreateInstance(Sprite **ppObj);

	virtual void OnDraw(lua_State *L, Gdiplus::Graphics &g, const Gdiplus::RectF &rcDirty);

	Gdiplus::RectF GetRect();
	static int GetRect(lua_State *L);
	static int GetRectT(lua_State *L);

	void SetRect(Gdiplus::RectF rect);
	static int SetRect(lua_State *L);

	Gdiplus::RectF GetAbsRect();
	static int GetAbsRect(lua_State *L);
	static int GetAbsRectT(lua_State *L);

	void Invalidate();

	void SetHostWnd(HostWindow *wnd);
	HostWindow *GetHostWindow();

	void SetVisible(bool v);
	static int SetVisible(lua_State *L);

	bool GetVisible();
	static int GetVisible(lua_State *L);

	void AddChild(Sprite *sp);
	static int AddChild(lua_State *L);

	virtual void OnMouseEvent( lua_State *L, MouseEvent *event );

	virtual void OnCapturedMouseEvent( lua_State *L, MouseEvent *event );

	virtual void OnKeyEvent( lua_State *L, UINT message, DWORD keyCode, DWORD flag );

	virtual void OnImeInput( lua_State *L, LPCTSTR text ) {}

	virtual void OnSetFocus();

	virtual void OnKillFocus();

	void EnableFocus(bool enable);
	static int EnableFocus(lua_State *L);

	void SetCapture();
	static int SetCapture(lua_State *L);

	void ReleaseCapture();
	static int ReleaseCapture(lua_State *L);

	void BringToFront();
	static int BringToFront(lua_State *L);

	void SetClipChildren(bool bClip);
	static int SetClipChildren(lua_State *L);

	bool GetClipChildren();
	static int GetClipChildren(lua_State *L);

	void TrackMouseLeave();
	static int TrackMouseLeave(lua_State *L);

	Sprite *DispatchMouseEvent(MouseEvent *event);

	void RemoveChild(Sprite *sp);
	static int RemoveChild(lua_State *L);

	Sprite *GetAncestor();
	static int GetAncestor(lua_State *L);

	Sprite *GetParent();
	static int GetParent(lua_State *L);

	Sprite *GetNextSprite();
	static int GetNextSprite(lua_State *L);

	Sprite *GetPrevSprite();
	static int GetPrevSprite(lua_State *L);

	Sprite *GetFirstSubSprite();
	static int GetFirstSubSprite(lua_State *L);

	Sprite *GetLastSubSprite();
	static int GetLastSubSprite(lua_State *L);

	RTTI_DECLARATIONS(Sprite, LuaObject)

	BEGIN_LUA_METHOD_MAP(Sprite)
		LUA_METHOD_ENTRY(SetRect)
		LUA_METHOD_ENTRY(GetRect)
		LUA_METHOD_ENTRY(GetRectT)
		LUA_METHOD_ENTRY(AddChild)
		LUA_METHOD_ENTRY(GetAbsRect)
		LUA_METHOD_ENTRY(GetAbsRectT)
		LUA_METHOD_ENTRY(EnableFocus)
		LUA_METHOD_ENTRY(SetCapture)
		LUA_METHOD_ENTRY(ReleaseCapture)
		LUA_METHOD_ENTRY(BringToFront)
		LUA_METHOD_ENTRY(GetVisible)
		LUA_METHOD_ENTRY(SetVisible)
		LUA_METHOD_ENTRY(GetClipChildren)
		LUA_METHOD_ENTRY(SetClipChildren)
		LUA_METHOD_ENTRY(TrackMouseLeave)
		LUA_METHOD_ENTRY(RemoveChild)
		LUA_METHOD_ENTRY(GetParent)
		LUA_METHOD_ENTRY(GetNextSprite)
		LUA_METHOD_ENTRY(GetPrevSprite)
		LUA_METHOD_ENTRY(GetFirstSubSprite)
		LUA_METHOD_ENTRY(GetLastSubSprite)
	END_LUA_METHOD_MAP()

private:
	Gdiplus::RectF m_rect;
	HostWindow *m_hostWnd;

	bool m_bVisible; // 这些bool或许可以弄成bit 节省内存
	bool m_bMouseIn;
	bool m_enableFocus;
	bool m_bClipChildren;

	Sprite *m_prevSibling; // 控件树结构 要高大上一点 zorder隐含在树结构中 避免排序
	Sprite *m_nextSibling; // 平级的的关系用双链表
	Sprite *m_firstChild;
	Sprite *m_lastChild;
	Sprite *m_parent;
	DISALLOW_COPY_AND_ASSIGN(Sprite);
};

class TextSprite : public Sprite
{
protected:
	TextSprite();
	virtual ~TextSprite();
public:
	static void CreateInstance(TextSprite **ppObj);

	void SetText(LPCTSTR text);
	static int SetText(lua_State *L);

	static int SetColor(lua_State *L);
	void SetColor(Gdiplus::Color clr);

	void SetHAlign(Gdiplus::StringAlignment align);
	static int SetHAlign(lua_State *L);

	void SetFont(LPCTSTR familyName, float emSize, int style);
	static int SetFont(lua_State *L);

	virtual void OnDraw(lua_State *L, Gdiplus::Graphics &g, const Gdiplus::RectF &rcDirty);

	BEGIN_LUA_METHOD_MAP(TextSprite)
		LUA_METHOD_ENTRY(SetText)
		LUA_METHOD_ENTRY(SetColor)
		LUA_METHOD_ENTRY(SetFont)
		LUA_CHAIN_METHOD_MAP(Sprite)
	END_LUA_METHOD_MAP()

private:
	std::wstring m_text;
	Gdiplus::Font *m_font;
	Gdiplus::Color m_color;
	Gdiplus::StringAlignment m_hAlign;
	Gdiplus::StringAlignment m_vAlign;
	DISALLOW_COPY_AND_ASSIGN(TextSprite);

};

class RectangleSprite : public Sprite
{
protected:
	RectangleSprite();
	virtual ~RectangleSprite();
public:
	static void CreateInstance(RectangleSprite **ppObj);

	void SetColor(Gdiplus::Color color);
	static int SetColor(lua_State *L);

	void SetBorderColor(Gdiplus::Color color);
	static int SetBorderColor(lua_State *L);

	virtual void OnDraw( lua_State *L, Gdiplus::Graphics &g , const Gdiplus::RectF &rcDirty);

	BEGIN_LUA_METHOD_MAP(RectangleSprite)
		LUA_METHOD_ENTRY(SetColor)
		LUA_METHOD_ENTRY(SetBorderColor)
		LUA_CHAIN_METHOD_MAP(Sprite)
	END_LUA_METHOD_MAP()

private:
	Gdiplus::Color m_color;
	Gdiplus::Color m_borderColor;
	DISALLOW_COPY_AND_ASSIGN(RectangleSprite);
};


