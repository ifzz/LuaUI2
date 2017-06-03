#pragma once

#include "LuaObject.h"

namespace cs {

class Sprite;
struct MouseEvent;

class SpriteLua : public LuaObject, public INotify
{
protected:
    virtual ~SpriteLua();
public:
    explicit SpriteLua(Sprite *sp);

    static int GetRect(lua_State *L);
    static int GetRectT(lua_State *L);
    static int SetRect(lua_State *L);
    static int GetAbsRect(lua_State *L);
    static int GetAbsRectT(lua_State *L);

    static int SetVisible(lua_State *L);
    static int GetVisible(lua_State *L);
    static int AddChild(lua_State *L);

    // TODO 这几个函数都有问题 dynamic_cast失败 对象模型有问题
    static int GetAncestor(lua_State *L);
    static int GetParent(lua_State *L);
    static int GetNextSprite(lua_State *L);
    static int GetPrevSprite(lua_State *L);
    static int GetFirstSubSprite(lua_State *L);
    static int GetLastSubSprite(lua_State *L);

    //
    static int EnableFocus(lua_State *L);
    static int SetCapture(lua_State *L);
    static int ReleaseCapture(lua_State *L);
    static int BringToFront(lua_State *L);
    static int SetClipChildren(lua_State *L);
    static int GetClipChildren(lua_State *L);
    static int TrackMouseLeave(lua_State *L);
    static int RemoveChild(lua_State *L);

    //
    static int ShowCaret(lua_State *L);
    static int SetCaretPos(lua_State *L);
    static int HideCaret(lua_State *L);

    static int Invalidate(lua_State *L);

    BEGIN_LUA_METHOD_MAP(Sprite)
        LUA_METHOD_ENTRY(SetRect)
        LUA_METHOD_ENTRY(GetRect)
        LUA_METHOD_ENTRY(GetRectT)
        LUA_METHOD_ENTRY(AddChild)
        LUA_METHOD_ENTRY(GetAbsRect)
        LUA_METHOD_ENTRY(GetAbsRectT)
        LUA_METHOD_ENTRY(EnableFocus)
        LUA_METHOD_ENTRY(SetCapture)
        LUA_METHOD_ENTRY(ReleaseCapture)
        LUA_METHOD_ENTRY(BringToFront)
        LUA_METHOD_ENTRY(GetVisible)
        LUA_METHOD_ENTRY(SetVisible)
        LUA_METHOD_ENTRY(GetClipChildren)
        LUA_METHOD_ENTRY(SetClipChildren)
        LUA_METHOD_ENTRY(TrackMouseLeave)
        LUA_METHOD_ENTRY(RemoveChild)
        LUA_METHOD_ENTRY(GetParent)
        LUA_METHOD_ENTRY(GetNextSprite)
        LUA_METHOD_ENTRY(GetPrevSprite)
        LUA_METHOD_ENTRY(GetFirstSubSprite)
        LUA_METHOD_ENTRY(GetLastSubSprite)
        LUA_METHOD_ENTRY(ShowCaret)
        LUA_METHOD_ENTRY(SetCaretPos)
        LUA_METHOD_ENTRY(HideCaret)
        LUA_METHOD_ENTRY(Invalidate)
    END_LUA_METHOD_MAP()

    virtual bool OnNotify(UINT idSender, void *sender, UINT idMessage, void *message) override;

    Object* GetCppSide();
    friend class Sprite;

private:
    Sprite *m_sp;
};

} // namespace cs
