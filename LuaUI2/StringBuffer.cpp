#include "stdafx.h"
#include "StringBuffer.h"

namespace cs {

StringBuffer::StringBuffer()
{
}


StringBuffer::~StringBuffer()
{
}

int StringBuffer::Append(lua_State *L)
{
    StringBuffer *thiz = CheckLuaObject<StringBuffer>(L, 1);
    int n = lua_gettop(L);
    for (int i = 1; i <= n; i++)
    {
        int type = lua_type(L, i);
        switch (type)
        {
        case LUA_TNUMBER:
            do
            {
                wchar_t buf[64];
                wsprintf(buf, L"%d", lua_tonumber(L, i));
                thiz->m_str.append(buf);
            } while (0);
            break;
        case LUA_TSTRING:
            do
            {
                CString str = LuaCheckWString(L, i);
                thiz->m_str.append((LPCWSTR)str);
            } while (0);
            break;
        case LUA_TUSERDATA:
            do
            {
                // 判断是否是一个 StringBuffer
                StringBuffer *other = CheckLuaObject<StringBuffer>(L, i);
                thiz->m_str.append(other->m_str);
            } while (0);
            break;
        default:
            do
            {
                CString str;
                str.Format(L"(%s:0x%x)", lua_typename(L, i), lua_topointer(L, i));
                thiz->m_str.append((LPCWSTR)str);
            } while (0);
            break;
        }
    }
    return 0;
}

int StringBuffer::Reserve(lua_State *L)
{
    StringBuffer *thiz = CheckLuaObject<StringBuffer>(L, 1);
    size_t count = luaL_checkinteger(L, 2);
    thiz->m_str.reserve(count);
    return 0;
}

int StringBuffer::At(lua_State *L)
{
    StringBuffer *thiz = CheckLuaObject<StringBuffer>(L, 1);
    size_t idx = luaL_checkinteger(L, 2);
    if (idx >= thiz->m_str.size())
    {
        luaL_error(L, "out of range: %d", idx);
    }
    wchar_t ch = thiz->m_str.at(idx + 1);
    lua_pushinteger(L, ch);
    return 1;
}

} // namespace cs
