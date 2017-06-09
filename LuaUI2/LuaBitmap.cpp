#include "stdafx.h"
#include "LuaBitmap.h"
#include "LBitmap.h"

namespace cs {

BitmapLua::BitmapLua(Bitmap *bmp)
{
    m_bmp = bmp;
}

BitmapLua::~BitmapLua()
{
}

int BitmapLua::LoadFromFile(lua_State *L)
{
    Bitmap *thiz = CheckLuaObject<BitmapLua>(L, 1)->m_bmp;
    CString path =  LuaCheckWString(L, 2);
    bool ret = thiz->LoadFromFile(path);
    lua_pushboolean(L, ret ? 1 : 0);
    return 1;
}

int BitmapLua::GetSize( lua_State *L )
{
    Bitmap *thiz = CheckLuaObject<BitmapLua>(L, 1)->m_bmp;
    auto size = thiz->GetSize();
	lua_pushnumber(L, size.Width);
    lua_pushnumber(L, size.Height);
	return 2;
}

int BitmapLua::GetFourStateInfo( lua_State *L )
{
    Bitmap *thiz = CheckLuaObject<BitmapLua>(L, 1)->m_bmp;
    std::vector<int> info;
    thiz->GetFourStateInfo(info);
    for (size_t i = 0; i < info.size(); i++)
    {
        lua_pushinteger(L, info.at(i));
    }
	return info.size();
}

int BitmapLua::GetNineInOneInfo( lua_State *L )
{
    Bitmap *thiz = CheckLuaObject<BitmapLua>(L, 1)->m_bmp;
    std::vector<int> info;
    thiz->GetNineInOneInfo(info);
    for (size_t i = 0; i < info.size(); i++)
    {
        lua_pushinteger(L, info.at(i));
    }
	return info.size();
}

Object * BitmapLua::GetCppSide()
{
    return m_bmp;
}

} // namespace cs
