#pragma once

#include "HostWindow.h"
#include "LuaObject.h"

namespace cs {

class HostWindow;

// TODO ¸Ä³ÉNativeWindow
class HostWindowLua : public LuaObject, public INotify, public IMessageHandler
{
protected:
    virtual ~HostWindowLua();
public:
    HostWindowLua();

    static int Create(lua_State *L);
    static int AttachSprite(lua_State *L);
    static int SetRect(lua_State *L);
    static int GetHWND(lua_State *L);

    BEGIN_LUA_METHOD_MAP(HostWindowLua)
        LUA_METHOD_ENTRY(Create)
        LUA_METHOD_ENTRY(AttachSprite)
        LUA_METHOD_ENTRY(SetRect)
        LUA_METHOD_ENTRY(GetHWND)
    END_LUA_METHOD_MAP()

    virtual Object * GetCppSide();

    virtual bool OnNotify(UINT idSender, void *sender, UINT idMessage, void *message);

    LRESULT HandleMessage(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, bool &bHandled);

private:
    HostWindow *m_wnd;
};

} // namespace cs
