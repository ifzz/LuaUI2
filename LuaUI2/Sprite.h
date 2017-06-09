/*
** Sprite.h The core element of LuaUI2
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/
#pragma once
#include "LuaObject.h"
#include "HostWindow.h"

namespace cs {

struct MouseEvent
{
	UINT message;
	float x;
	float y;
	UINT flag; // ctrl shift atl
	float delta; // wheel
};

struct KeyEvent
{
    DWORD keyCode;
    DWORD flag;
};

struct PaintEvent
{
    Gdiplus::Graphics *graphics;
    Gdiplus::RectF rcDirty;
};

class SpriteLua;

class Sprite : public Object
{
protected:
	virtual ~Sprite(void);
public:
    Sprite(void);

	virtual void OnDraw(Gdiplus::Graphics &g, const Gdiplus::RectF &rcDirty);
    //virtual void ClientDraw(Gdiplus::Graphics &g, const Gdiplus::RectF &rcDirty) {}

	Gdiplus::RectF GetRect();

	void SetRect(Gdiplus::RectF rect);

	Gdiplus::RectF GetAbsRect();

	void Invalidate();

	void SetHostWnd(HostWindow *wnd);
	HostWindow *GetHostWindow();

	void SetVisible(bool v);

	bool GetVisible();

	void AddChild(Sprite *sp);

	virtual void OnMouseEvent( lua_State *L, MouseEvent *event );

    enum Events
    {
        eSpriteFirst,
        ePaint,

        eMouseFirst,
        eMouseMove,
        eMouseEnter,
        eMouseLeave,
        eMouseWheel,
        eLBtnDown,
        eLBtnUp,
        eRBtnDown,
        eRBtnUp,
        eMouseLast,

        eKeyDown,
        eKeyUp,
        eCharInput,
        eImeInput,
        eSetFocus,
        eKillFocus,
        eSizeChanged,
        eSpriteLast     // 子类从这里开始
    };

	virtual void OnCapturedMouseEvent( lua_State *L, MouseEvent *event );

	virtual void OnKeyEvent( lua_State *L, UINT message, DWORD keyCode, DWORD flag );

	virtual void OnImeInput( lua_State *L, LPCTSTR text );

	void EnableFocus(bool enable);

	void SetCapture();

	void ReleaseCapture();

	void BringToFront();

	void SetClipChildren(bool bClip);

	bool GetClipChildren();

	void TrackMouseLeave();

	Sprite *DispatchMouseEvent(MouseEvent *ev);

	void RemoveChild(Sprite *sp);

	Sprite *GetAncestor();

	Sprite *GetParent();

	Sprite *GetNextSprite();

	Sprite *GetPrevSprite();

	Sprite *GetFirstSubSprite(); // TODO 改成GetFirstChild 麻德 不要sub

	Sprite *GetLastSubSprite();

    void ShowCaret();
    void SetCaretPos(Gdiplus::RectF rc);
    void HideCaret();

    virtual LuaObject *GetLuaSide();

    void SendNotify(UINT idMessage, void *message);

    enum State {
        sVisible = 0x01,
        sMouseIn = 0x02,
        sEnableFocus = 0x04,
        sClipChildren = 0x08,
        sShowCaret = 0x10
    }; // TODO

private:
	Gdiplus::RectF m_rect;
	HostWindow *m_hostWnd;

	bool m_bVisible; // 这些bool或许可以弄成bit 节省内存
	bool m_bMouseIn;
	bool m_enableFocus;
	bool m_bClipChildren;
    bool m_bShowCaret;

	Sprite *m_prevSibling; // 控件树结构 要高大上一点 zorder隐含在树结构中 避免排序
	Sprite *m_nextSibling; // 平级的的关系用双链表
	Sprite *m_firstChild;
	Sprite *m_lastChild;
	Sprite *m_parent;

    UINT m_id;

	SpriteLua *m_luaSide;

	DISALLOW_COPY_AND_ASSIGN(Sprite);
};

} // namespace cs