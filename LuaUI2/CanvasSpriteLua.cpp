#include "stdafx.h"
#include "CanvasSpriteLua.h"
#include "LuaBitmap.h"
#include "LBitmap.h"

extern lua_State *g_L;

namespace cs {

CanvasSpriteLua::CanvasSpriteLua(CanvasSprite *c)
{
    canvas = c;
}

CanvasSpriteLua::~CanvasSpriteLua()
{
}

int CanvasSpriteLua::DrawLine(lua_State *L)
{
    CanvasSprite *thiz = CheckLuaObject<CanvasSpriteLua>(L, 1)->canvas;
    Gdiplus::PointF p1, p2;
    p1.X = (float)luaL_checknumber(L, 2);
    p1.Y = (float)luaL_checknumber(L, 3);
    p2.X = (float)luaL_checknumber(L, 4);
    p2.Y = (float)luaL_checknumber(L, 5);
    float width = (float)luaL_checknumber(L, 6);
    thiz->DrawLine(p1, p2, width);

    return 0;
}

int CanvasSpriteLua::DrawRect(lua_State *L)
{
    CanvasSprite *thiz = CheckLuaObject<CanvasSpriteLua>(L, 1)->canvas;
    Gdiplus::RectF rc = luaL_checkrectf(L, 2);
    thiz->DrawRect(rc);
    return 0;
}

int CanvasSpriteLua::DrawChar(lua_State *L)
{
    CanvasSprite *thiz = CheckLuaObject<CanvasSpriteLua>(L, 1)->canvas;

    wchar_t ch = luaL_checkinteger(L, 2);
    Gdiplus::PointF p1;
    p1.X = (float)luaL_checknumber(L, 3);
    p1.Y = (float)luaL_checknumber(L, 4);
    thiz->DrawChar(p1, ch);
    return 0;
}

int CanvasSpriteLua::MeasureChar(lua_State *L)
{
    CanvasSprite *thiz = CheckLuaObject<CanvasSpriteLua>(L, 1)->canvas;
    wchar_t ch = luaL_checkinteger(L, 2);
    Gdiplus::RectF rc = thiz->MeasureChar(ch);
    lua_pushnumber(L, rc.Width);
    lua_pushnumber(L, rc.Height);
    return 2;
}

int CanvasSpriteLua::DrawString(lua_State *L)
{
    CanvasSprite *thiz = CheckLuaObject<CanvasSpriteLua>(L, 1)->canvas;
    CStringW text = luaL_checkwstring(L, 2);
    Gdiplus::RectF rc = luaL_checkrectf(L, 3);
    UINT hAlign = 0;
    if (lua_gettop(L) >= 4)
    {
        hAlign = luaL_checkinteger(L, 4);
    }
    UINT vAlign = 0;
    if (lua_gettop(L) >= 5)
    {
        vAlign = luaL_checkinteger(L, 5);
    }
    thiz->DrawString(text, rc, hAlign, vAlign);
    return 0;
}

// TODO 这个没测
int CanvasSpriteLua::MeasureString(lua_State *L)
{
    CanvasSprite *thiz = CheckLuaObject<CanvasSpriteLua>(L, 1)->canvas;

    CStringW str = luaL_checkwstring(L, 2);
    Gdiplus::RectF rc = luaL_checkrectf(L, 3);
    //const char* align = luaL_checkstring(L, 4);
    Gdiplus::RectF rcOut;
    thiz->MeasureString(str, rc, &rcOut);
    RectF2LuaRect(L, rcOut);
    return 1;
}

int CanvasSpriteLua::DrawBitmap(lua_State *L)
{
    CanvasSprite *thiz = CheckLuaObject<CanvasSpriteLua>(L, 1)->canvas;
    BitmapLua *bmp_l = CheckLuaObject<BitmapLua>(L, 2);
    Bitmap *bmp = dynamic_cast<Bitmap *>(bmp_l->GetCppSide());
    //if (!thiz->m_pGraphics)
    //    luaL_error(L, "no graphics");
    if (lua_istable(L, 3) && lua_istable(L, 4))
    {
        Gdiplus::RectF rcSrc = luaL_checkrectf(L, 3);
        Gdiplus::RectF rcDst = luaL_checkrectf(L, 4);
        thiz->DrawBitmap(bmp, rcSrc, rcDst);
    }
    else if (lua_gettop(L) == 8)
    {
        float x = (float)luaL_checknumber(L, 3);
        float y = (float)luaL_checknumber(L, 4);
        float srcX = (float)luaL_checknumber(L, 5);
        float srcY = (float)luaL_checknumber(L, 6);
        float srcWidth = (float)luaL_checknumber(L, 7);
        float srcHeight = (float)luaL_checknumber(L, 8);
        thiz->DrawBitmap(bmp, x, y, srcX, srcY, srcWidth, srcHeight);
    }
    else if (lua_gettop(L) == 4)
    {
        float x = (float)luaL_checknumber(L, 3);
        float y = (float)luaL_checknumber(L, 4);
        thiz->DrawBitmap(bmp, x, y);
    }
    else
    {
        luaL_error(L, "type error");
    }
    return 0;
}

int CanvasSpriteLua::SetColor(lua_State *L)
{
    CanvasSprite *thiz = CheckLuaObject<CanvasSpriteLua>(L, 1)->canvas;
    BYTE r, g, b;
    r = (BYTE)luaL_checkinteger(L, 2);
    g = (BYTE)luaL_checkinteger(L, 3);
    b = (BYTE)luaL_checkinteger(L, 4);
    Gdiplus::Color color(r, g, b);
    thiz->SetColor(color);
    return 0;
}

int CanvasSpriteLua::SetFont(lua_State *L)
{
    CanvasSprite *thiz = CheckLuaObject<CanvasSpriteLua>(L, 1)->canvas;
    CStringW family = luaL_checkwstring(L, 2);
    float size = (float)luaL_checknumber(L, 3);
    int style = luaL_checkinteger(L, 4);
    if (!thiz->SetFont(family, size, style))
    {
        luaL_error(L, "font create failed"); // TODO 其他函数要检查m_font
    }
    return 0;
}

Object * CanvasSpriteLua::GetCppSide()
{
    return canvas;
}

} // namespace cs
