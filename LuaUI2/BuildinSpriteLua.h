#pragma once

#include "SpriteLua.h"

namespace cs {

class TextSprite;

class TextSpriteLua : public SpriteLua
{
protected:
    ~TextSpriteLua();
public:
    TextSpriteLua(TextSprite *sp);

    static int SetText(lua_State *L);
    static int SetColor(lua_State *L);
    static int SetHAlign(lua_State *L);
    static int SetFont(lua_State *L);

    virtual Object *GetCppSide() override;

    BEGIN_LUA_METHOD_MAP(TextSprite)
        LUA_METHOD_ENTRY(SetText)
        LUA_METHOD_ENTRY(SetColor)
        LUA_METHOD_ENTRY(SetHAlign)
        LUA_METHOD_ENTRY(SetFont)
        LUA_CHAIN_METHOD_MAP(SpriteLua)
    END_LUA_METHOD_MAP()

private:
    TextSprite *m_sp;
    DISALLOW_COPY_AND_ASSIGN(TextSpriteLua);
};

class RectangleSprite;

class RectangleSpriteLua : public SpriteLua
{
protected:
    ~RectangleSpriteLua();
public:
    RectangleSpriteLua(RectangleSprite *sp); 

    static int SetColor(lua_State *L);
    static int SetBorderColor(lua_State *L);

    BEGIN_LUA_METHOD_MAP(RectangleSprite)
        LUA_METHOD_ENTRY(SetColor)
        LUA_METHOD_ENTRY(SetBorderColor)
        LUA_CHAIN_METHOD_MAP(SpriteLua)
    END_LUA_METHOD_MAP()
private:
    RectangleSprite *m_sp;
    DISALLOW_COPY_AND_ASSIGN(RectangleSpriteLua);
};



}