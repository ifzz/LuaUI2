#pragma once

#include "SpriteLua.h"

namespace cs {

class EditSprite;

class EditSpriteLua : public SpriteLua
{
protected:
    virtual ~EditSpriteLua();
public:
    explicit EditSpriteLua(EditSprite *sp);


    BEGIN_LUA_METHOD_MAP(EditSprite)
    	//LUA_METHOD_ENTRY(GetText)
    	LUA_CHAIN_METHOD_MAP(SpriteLua)
    END_LUA_METHOD_MAP()

    virtual bool OnNotify(UINT idMessage, void *message) override;

    virtual Object * GetCppSide() override;

private:
    EditSprite *m_sp;
};

} // namespace cs
