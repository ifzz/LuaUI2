#pragma once

#include "TextEdit.h"
#include "LuaObject.h"
#include "SpriteLua.h"

namespace cs {

class TextEditLua : public SpriteLua
{
public:
    TextEditLua(TextEdit *tx);
    virtual ~TextEditLua();

    BEGIN_LUA_METHOD_MAP(TextEdit)
        //LUA_METHOD_ENTRY(GetText)
        LUA_CHAIN_METHOD_MAP(SpriteLua)
    END_LUA_METHOD_MAP()
};

} // namespace cs
