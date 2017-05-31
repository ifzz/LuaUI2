#include "stdafx.h"
#include "LuaBitmap.h"

namespace cs {

LuaBitmap::LuaBitmap()
{
}

LuaBitmap::~LuaBitmap()
{
}

int LuaBitmap::LoadFromFile(lua_State *L)
{
    LBitmap *thiz = CheckLuaObject<LuaBitmap>(L, 1);
    CString path =  luaL_checkwstring(L, 2);
    bool ret = thiz->LoadFromFile(path);
    lua_pushboolean(L, ret ? 1 : 0);
    return 1;
}

int LuaBitmap::GetSize( lua_State *L )
{
	LBitmap *thiz = CheckLuaObject<LuaBitmap>(L, 1); 
    auto size = thiz->GetSize();
	lua_pushnumber(L, size.Width);
    lua_pushnumber(L, size.Height);
	return 2;
}

int LuaBitmap::GetFourStateInfo( lua_State *L )
{
    LBitmap *thiz = CheckLuaObject<LuaBitmap>(L, 1);
    std::vector<int> info;
    thiz->GetFourStateInfo(info);
    for (size_t i = 0; i < info.size(); i++)
    {
        lua_pushinteger(L, info.at(i));
    }
	return info.size();
}

int LuaBitmap::GetNineInOneInfo( lua_State *L )
{
    LBitmap *thiz = CheckLuaObject<LuaBitmap>(L, 1);
    std::vector<int> info;
    thiz->GetNineInOneInfo(info);
    for (size_t i = 0; i < info.size(); i++)
    {
        lua_pushinteger(L, info.at(i));
    }
	return info.size();
}

} // namespace cs
