#include "stdafx.h"
#include "HostWindowLua.h"
#include "SpriteLua.h"

extern lua_State *g_L; // GUI线程用的主lua state

namespace cs {

HostWindowLua::HostWindowLua(HostWindow *wnd)
{
    m_wnd = wnd;
}

HostWindowLua::~HostWindowLua()
{
}

int HostWindowLua::Create(lua_State *L)
{
    HostWindow *thiz = CheckLuaObject<HostWindowLua>(L, 1)->m_wnd;

    if (thiz->GetHWND() != NULL)
    {
        luaL_error(L, "HostWindow Create twice");
        return 0;
    }
    DWORD exStyle = luaL_checkinteger(L, 2);
    DWORD style = luaL_checkinteger(L, 3);
    Gdiplus::RectF rc = LuaCheckRectF(L, 4);

    HostWindow *parent = NULL;
    if (lua_isuserdata(L, 5))
    {
        parent = CheckLuaObject<HostWindowLua>(L, 5)->m_wnd;
    }
    thiz->Create(parent, rc, style, exStyle);

    return 0;
}

int HostWindowLua::AttachSprite(lua_State *L)
{
    HostWindow *thiz = CheckLuaObject<HostWindowLua>(L, 1)->m_wnd;
    Sprite *sprite = (Sprite *)CheckLuaObject<SpriteLua>(L, 2)->GetCppSide();
    thiz->AttachSprite(sprite);
    return 0;
}

int HostWindowLua::SetRect(lua_State *L)
{
    HostWindow *thiz = CheckLuaObject<HostWindowLua>(L, 1)->m_wnd;
    Gdiplus::RectF rc = LuaCheckRectF(L, 2);

    if (thiz->GetHWND())
    {
        thiz->SetRect(rc);
    }
    else
    {
        luaL_error(L, "HWND == NULL");
    }
    return 0;
}

/*
void HostWindowLua::OnSize(float cx, float cy, DWORD flag)
{
    lua_State *L = g_L;
    lua_pushnumber(L, cx);
    lua_pushnumber(L, cy);
    lua_pushinteger(L, flag);
    InvokeCallback(L, "OnSize", 3, 0);
}

void HostWindowLua::OnDestroy()
{
    lua_State *L = g_L;
    InvokeCallback(L, "OnDestroy", 0, 0);
}
*/

int HostWindowLua::GetHWND(lua_State *L)
{
    HostWindow *thiz = CheckLuaObject<HostWindowLua>(L, 1)->m_wnd;
    HWND hwnd = thiz->GetHWND();
    lua_pushinteger(L, reinterpret_cast<int>(hwnd));
    return 1;
}

Object * HostWindowLua::GetCppSide()
{
    return m_wnd;
}

bool HostWindowLua::OnNotify(UINT idMessage, void *message)
{
    return false;
}

LRESULT HostWindowLua::HandleMessage(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, bool &bHandled)
{
    lua_State *L = GetLuaState();
    switch (message)
    {
    case WM_SIZE:
        do {
            float cx = (float)(short)LOWORD(lparam);
            float cy = (float)(short)HIWORD(lparam);
            DWORD flag = wparam;
            lua_pushnumber(L, cx);
            lua_pushnumber(L, cy);
            lua_pushinteger(L, flag);
            bHandled = InvokeCallback(L, "OnSize", 3, 0);
        } while (0);
        break;
    case WM_DESTROY:
        do {
            bHandled = InvokeCallback(L, "OnDestroy", 0, 0);
        } while (0);
        break;
    default:
        bHandled = false;
        break;
    }
    return 0; // FIXME
}

} // namespace cs
