#pragma once
#include "LuaObject.h"
#include "CanvasSprite.h"

namespace cs {

class CanvasSpriteLua : public LuaObject
{
public:
    explicit CanvasSpriteLua(CanvasSprite *c);
    virtual ~CanvasSpriteLua();

    static int DrawLine(lua_State *L);
    static int DrawRect(lua_State *L);
    static int DrawChar(lua_State *L);
    static int MeasureChar(lua_State *L);
    static int DrawString(lua_State *L);
    static int MeasureString(lua_State *L);
    static int DrawBitmap(lua_State *L);
    static int SetColor(lua_State *L);
    static int SetFont(lua_State *L);


    BEGIN_LUA_METHOD_MAP(CanvasSpriteLua)
        LUA_METHOD_ENTRY(DrawChar)
        LUA_METHOD_ENTRY(DrawBitmap)
        LUA_METHOD_ENTRY(DrawLine)
        LUA_METHOD_ENTRY(DrawRect)
        LUA_METHOD_ENTRY(DrawString)
        LUA_METHOD_ENTRY(MeasureChar)
        LUA_METHOD_ENTRY(MeasureString)
        LUA_METHOD_ENTRY(SetColor)
        LUA_METHOD_ENTRY(SetFont)
    END_LUA_METHOD_MAP()

    virtual Object * GetCppSide();

private:
    CanvasSprite *canvas;
};

} // namespace cs
