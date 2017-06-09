#include "stdafx.h"
#include "EditSpriteLua.h"
#include "EditSprite.h"

namespace cs {

EditSpriteLua::EditSpriteLua(EditSprite *sp) :
    SpriteLua(sp)
{
    m_sp = NULL;
}


EditSpriteLua::~EditSpriteLua()
{
}

bool cs::EditSpriteLua::OnNotify(UINT idMessage, void *message)
{
    return SpriteLua::OnNotify(idMessage, message);
}

Object * cs::EditSpriteLua::GetCppSide()
{
    return m_sp;
}

} // namespace cs
