#pragma once
#include "LuaObject.h"

namespace cs {

class StringBuffer :
    public virtual Object, public virtual LuaObject
{
protected:
    virtual ~StringBuffer();

public:
    StringBuffer();

    static int Append(lua_State *L);
    static int Reserve(lua_State *L);
    static int At(lua_State *L);

    BEGIN_LUA_METHOD_MAP(StringBuffer)
        LUA_METHOD_ENTRY(Append)
        LUA_METHOD_ENTRY(Reserve)
        LUA_METHOD_ENTRY(At)
    END_LUA_METHOD_MAP()

private:
    std::wstring m_str;
};

} // namespace cs
