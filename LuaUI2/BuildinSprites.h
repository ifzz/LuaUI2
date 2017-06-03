#pragma once

#include "Sprite.h"

namespace cs {

class TextSpriteLua;

class TextSprite : public Sprite
{
protected:
    virtual ~TextSprite();
public:
    TextSprite();

    void SetText(LPCTSTR text);

    void SetColor(Gdiplus::Color clr);

    void SetHAlign(Gdiplus::StringAlignment align);

    void SetFont(LPCTSTR familyName, float emSize, int style);

    void OnDraw(Gdiplus::Graphics &g, const Gdiplus::RectF &rcDirty);

    virtual LuaObject *GetLuaSide() override;

private:
    std::wstring m_text;
    Gdiplus::Font *m_font;
    Gdiplus::Color m_color;
    Gdiplus::StringAlignment m_hAlign;
    Gdiplus::StringAlignment m_vAlign;

    TextSpriteLua *m_luaSide;
    DISALLOW_COPY_AND_ASSIGN(TextSprite);
};

class RectangleSpriteLua;

class RectangleSprite : public Sprite
{
protected:
    virtual ~RectangleSprite();
public:
    RectangleSprite();

    void SetColor(Gdiplus::Color color);

    void SetBorderColor(Gdiplus::Color color);

    void OnDraw(Gdiplus::Graphics &g, const Gdiplus::RectF &rcDirty) override;

    virtual LuaObject * GetLuaSide() override;

private:
    Gdiplus::Color m_color;
    Gdiplus::Color m_borderColor;

    RectangleSpriteLua *m_luaSide;
    DISALLOW_COPY_AND_ASSIGN(RectangleSprite);
};

} // namespace cs
