#pragma once

#include "Sprite.h"

namespace cs {

class TextEdit : public virtual Sprite
{
protected:
    virtual ~TextEdit();
public:
    TextEdit();

    virtual void OnImeInput(lua_State *L, LPCTSTR text);

    virtual void ClientDraw(Gdiplus::Graphics &g, const Gdiplus::RectF &rcDirty);

    virtual void OnMouseEvent(lua_State *L, MouseEvent *event){}

    virtual void OnCapturedMouseEvent(lua_State *L, MouseEvent *event){}

    virtual void OnKeyEvent(lua_State *L, UINT message, DWORD keyCode, DWORD flag){}

    virtual void OnSetFocus(){}

    virtual void OnKillFocus(){}

};

} // namespace cs