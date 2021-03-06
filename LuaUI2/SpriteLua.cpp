#include "stdafx.h"
#include "SpriteLua.h"
#include "Sprite.h"
#include "CanvasSpriteLua.h"

extern lua_State *g_L; // GUI线程用的主lua state

namespace cs {

SpriteLua::SpriteLua(Sprite *sp)
{
    m_sp = sp;
}


SpriteLua::~SpriteLua()
{
}

int SpriteLua::GetRect(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    Gdiplus::RectF rc = thiz->GetRect();
    lua_pushnumber(L, rc.X);
    lua_pushnumber(L, rc.Y);
    lua_pushnumber(L, rc.Width);
    lua_pushnumber(L, rc.Height);
    return 4;
}

int SpriteLua::GetRectT(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    Gdiplus::RectF rc = thiz->GetRect();
    RectF2LuaRect(L, rc);
    return 1;
}

int SpriteLua::SetRect(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    Gdiplus::RectF rc;
    if (lua_istable(L, 2))
    {
        lua_getfield(L, 2, "x");
        rc.X = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 2, "y");
        rc.Y = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 2, "w");
        rc.Width = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 2, "h");
        rc.Height = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
    }
    else
    {
        rc.X = (float)luaL_checknumber(L, 2);
        rc.Y = (float)luaL_checknumber(L, 3);
        rc.Width = (float)luaL_checknumber(L, 4);
        rc.Height = (float)luaL_checknumber(L, 5);

    }
    thiz->SetRect(rc);
    return 0;
}

int SpriteLua::GetAbsRect(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    Gdiplus::RectF rc = thiz->GetAbsRect();
    lua_pushnumber(L, rc.X);
    lua_pushnumber(L, rc.Y);
    lua_pushnumber(L, rc.Width);
    lua_pushnumber(L, rc.Height);
    return 4;
}

int SpriteLua::GetAbsRectT(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    Gdiplus::RectF rc = thiz->GetAbsRect();
    RectF2LuaRect(L, rc);
    return 1;
}

int SpriteLua::SetVisible(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    int b = lua_toboolean(L, 2);
    thiz->SetVisible(b != 0);
    return 0;
}

int SpriteLua::GetVisible(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    bool bVisible = thiz->GetVisible();
    lua_pushboolean(L, bVisible ? 1 : 0);
    return 1;
}

int SpriteLua::AddChild(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    Sprite *sprite = CheckLuaObject<SpriteLua>(L, 2)->m_sp;
    thiz->AddChild(sprite);
    return 0;
}

int SpriteLua::GetAncestor(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    Sprite *ancestor = thiz->GetAncestor();
    if (!ancestor)
        return 0;
    ancestor->GetLuaSide()->PushToLua(L);
    return 1;
}

int SpriteLua::GetParent(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    if (!thiz->GetParent())
        return 0;
    thiz->GetParent()->GetLuaSide()->PushToLua(L);
    return 1;
}

int SpriteLua::GetNextSprite(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    if (!thiz->GetNextSprite())
        return 0;
    Sprite *sp = thiz->GetNextSprite();
    sp->GetLuaSide()->PushToLua(L);
    return 1;
}

int SpriteLua::GetPrevSprite(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    if (!thiz->GetPrevSprite())
        return 0;
    Sprite *sp = thiz->GetPrevSprite();
    sp->GetLuaSide()->PushToLua(L);
    return 1;
}

int SpriteLua::GetFirstSubSprite(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    if (!thiz->GetFirstSubSprite())
        return 0;
    Sprite *sp = thiz->GetFirstSubSprite();
    sp->GetLuaSide()->PushToLua(L);
    return 1;
}

int SpriteLua::GetLastSubSprite(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    if (!thiz->GetLastSubSprite())
        return 0;
    Sprite *sp = thiz->GetLastSubSprite();
    sp->GetLuaSide()->PushToLua(L);
    return 1;
}

int SpriteLua::EnableFocus(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    bool enbale = false;
    if (lua_isnone(L, 2))
    {
        enbale = true;
    }
    else if (lua_isboolean(L, 2))
    {
        enbale = (lua_toboolean(L, 2) != 0);
    }
    else
    {
        luaL_error(L, "need a boolean value.");
    }
    thiz->EnableFocus(enbale);
    return 0;
}

int SpriteLua::SetCapture(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    thiz->SetCapture();
    return 0;
}

int SpriteLua::ReleaseCapture(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    thiz->ReleaseCapture();
    return 0;
}

int SpriteLua::BringToFront(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    thiz->BringToFront();
    return 0;
}

int SpriteLua::SetClipChildren(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    bool bClip = (lua_toboolean(L, 2) != 0);
    thiz->SetClipChildren(bClip);
    return 0;
}

int SpriteLua::GetClipChildren(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    bool bClip = thiz->GetClipChildren();
    lua_pushboolean(L, bClip ? 1 : 0);
    return 1;
}

int SpriteLua::TrackMouseLeave(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    thiz->TrackMouseLeave();
    return 0;
}

int SpriteLua::RemoveChild(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    Sprite *sp = CheckLuaObject<SpriteLua>(L, 2)->m_sp;
    thiz->RemoveChild(sp);
    return 0;
}

//////////////////////////////////////////////////////////////////////////

bool SpriteLua::OnNotify(UINT idMessage, void *message)
{
    lua_State *L = g_L;
    switch (idMessage)
    {
    case Sprite::ePaint:
        do {
            PaintEvent *ev = static_cast<PaintEvent *>(message);
            CanvasSprite *canvas = new CanvasSprite(ev->graphics);
            canvas->GetLuaSide()->PushToLua(L);
            bool bProcessed = InvokeCallback(L, "OnDraw", 1, 0);
            canvas->Close(); // make the object useless, forbid lua code reuse it.
            canvas->Unref();
            return bProcessed;
        } while (0);
        break;
    case Sprite::eSizeChanged:
        do {
            Gdiplus::SizeF *size = static_cast<Gdiplus::SizeF *>(message);
            lua_pushnumber(L, size->Width);
            lua_pushnumber(L, size->Height);
            return InvokeCallback(L, "OnSize", 2, 0);
        } while (0);
        break;
    case Sprite::eMouseMove:
        do {
            MouseEvent *ev = static_cast<MouseEvent *>(message);
            lua_pushnumber(L, ev->x);
            lua_pushnumber(L, ev->y);
            lua_pushinteger(L, ev->flag);
            return InvokeCallback(L, "OnMouseMove", 3, 0);
        } while (0);
        break;
    case Sprite::eMouseEnter:
        return InvokeCallback(L, "OnMouseEnter", 1, 0);
        break;
    case Sprite::eMouseLeave:
        return InvokeCallback(L, "OnMouseLeave", 0, 0);
        break;
    case Sprite::eMouseWheel:
        do {
            MouseEvent *ev = static_cast<MouseEvent *>(message);
            lua_pushnumber(L, ev->x);
            lua_pushnumber(L, ev->y);
            lua_pushinteger(L, ev->flag);
            lua_pushnumber(L, ev->delta);
            return InvokeCallback(L, "OnMouseWheel", 4, 0);
        } while (0);
        break;
    case Sprite::eLBtnDown:
        do {
            MouseEvent *ev = static_cast<MouseEvent *>(message);
            lua_pushnumber(L, ev->x);
            lua_pushnumber(L, ev->y);
            lua_pushinteger(L, ev->flag);
            return InvokeCallback(L, "OnLButtonDown", 3, 0);
        } while (0);
        break;
    case Sprite::eLBtnUp:
        do {
            MouseEvent *ev = static_cast<MouseEvent *>(message);
            lua_pushnumber(L, ev->x);
            lua_pushnumber(L, ev->y);
            lua_pushinteger(L, ev->flag);
            return InvokeCallback(L, "OnLButtonUp", 3, 0);
        } while (0);
        break;
    case Sprite::eSetFocus:
        return InvokeCallback(L, "OnSetFocus", 0, 0);
        break;
    case Sprite::eKillFocus:
        return InvokeCallback(L, "OnKillFocus", 0, 0);
        break;
    case Sprite::eCharInput:
        do 
        {
            KeyEvent *ev = static_cast<KeyEvent *>(message);
            lua_pushinteger(L, ev->keyCode);
            lua_pushinteger(L, ev->flag);
            return InvokeCallback(L, "OnChar", 2, 0);
        } while (0);
    default:
        return false;
    }
}

int SpriteLua::ShowCaret(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    thiz->ShowCaret();
    return 0;
}

// rect 指定光标的位置和大小
int SpriteLua::SetCaretPos(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    Gdiplus::RectF rc = LuaCheckRectF(L, 2);
    thiz->SetCaretPos(rc);
    return 0;
}

int SpriteLua::HideCaret(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    thiz->HideCaret();
    return 0;
}

int SpriteLua::Invalidate(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<SpriteLua>(L, 1)->m_sp;
    // TODO 这里加上指定刷新区域
    thiz->Invalidate();
    return 0;
}

Object* SpriteLua::GetCppSide()
{
    return m_sp;
}

} // namespace cs
