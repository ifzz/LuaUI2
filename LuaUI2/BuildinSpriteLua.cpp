#include "stdafx.h"
#include "BuildinSprites.h"
#include "BuildinSpriteLua.h"

namespace cs {

TextSpriteLua::TextSpriteLua(TextSprite *sp):
    SpriteLua(sp)
{
    m_sp = sp;
}

TextSpriteLua::~TextSpriteLua()
{

}

Object * TextSpriteLua::GetCppSide()
{
    return m_sp;    
}

int TextSpriteLua::SetText(lua_State *L)
{
    TextSprite *thiz = CheckLuaObject<TextSpriteLua>(L, 1)->m_sp;
    CString str = LuaCheckWString(L, 2);
    thiz->SetText(str);
    return 0;
}

int TextSpriteLua::SetFont(lua_State *L)
{
    TextSprite *thiz = CheckLuaObject<TextSpriteLua>(L, 1)->m_sp;
    CString face = LuaCheckWString(L, 2);
    float size = (float)luaL_checknumber(L, 3);
    int style = luaL_checkinteger(L, 4);
    thiz->SetFont(face, size, style);
    return 0;
}

int TextSpriteLua::SetColor(lua_State *L)
{
    TextSprite *thiz = CheckLuaObject<TextSpriteLua>(L, 1)->m_sp;
    Gdiplus::Color color = LuaCheckColor(L, 2);
    thiz->SetColor(color);
    return 0;
}


RectangleSpriteLua::RectangleSpriteLua(RectangleSprite *sp):
    SpriteLua(sp)
{
    m_sp = sp;    
}

RectangleSpriteLua::~RectangleSpriteLua()
{

}

int RectangleSpriteLua::SetColor(lua_State *L)
{
    RectangleSprite *thiz = CheckLuaObject<RectangleSpriteLua>(L, 1)->m_sp;
    Gdiplus::Color color = LuaCheckColor(L, 2);
    thiz->SetColor(color);
    return 0;
}

int RectangleSpriteLua::SetBorderColor(lua_State *L)
{
    RectangleSprite *thiz = CheckLuaObject<RectangleSpriteLua>(L, 1)->m_sp;
    Gdiplus::Color color = LuaCheckColor(L, 2);
    thiz->SetBorderColor(color);
    return 0;
}

} // namespace
