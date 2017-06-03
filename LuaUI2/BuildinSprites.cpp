#include "stdafx.h"
#include "BuildinSprites.h"
#include "BuildinSpriteLua.h"
#include "ResourceManager.h"

extern lua_State *g_L; // GUI线程用的主lua state

namespace cs {

TextSprite::TextSprite() :m_color(255, 0, 255)
{
    m_font = NULL;
    SetFont(L"宋体", 12, Gdiplus::FontStyleRegular);
    m_vAlign = Gdiplus::StringAlignmentCenter;
    m_hAlign = Gdiplus::StringAlignmentCenter;
}

TextSprite::~TextSprite()
{
    delete m_font;
    m_font = NULL;
}

void TextSprite::SetText(LPCTSTR text)
{
    m_text = text;
    Sprite::Invalidate();
}

void TextSprite::SetFont(LPCTSTR familyName, float emSize, int style)
{
    //Gdiplus::Font *font = ResourceManager::Instance()->GetFont(familyName, emSize, style);
    Gdiplus::Font *font = new Gdiplus::Font(familyName, emSize, style);
    ATLASSERT(font);
    m_font = font;
}

void TextSprite::SetColor(Gdiplus::Color clr)
{
    m_color = clr;
    Sprite::Invalidate();
}

void TextSprite::OnDraw(Gdiplus::Graphics &g, const Gdiplus::RectF &rcDirty)
{
    Gdiplus::SolidBrush brush(m_color);
    Gdiplus::StringFormat format;
    format.SetAlignment(m_hAlign);
    format.SetLineAlignment(m_vAlign);
    Gdiplus::RectF rc = Sprite::GetRect();
    rc.X = 0.0f;
    rc.Y = 0.0f;
    g.DrawString(m_text.c_str(), m_text.length(), m_font, rc, &format, &brush);
}

void TextSprite::SetHAlign(Gdiplus::StringAlignment align)
{
    m_hAlign = align;
}

LuaObject * TextSprite::GetLuaSide()
{
    return m_luaSide;
}

RectangleSprite::RectangleSprite() :m_color(255, 0, 255), m_borderColor(255, 0, 255)
{
}

RectangleSprite::~RectangleSprite()
{
}

void RectangleSprite::SetColor(Gdiplus::Color color)
{
    if (m_color.GetValue() != color.GetValue())
    {
        m_color = color;
        Sprite::Invalidate();
    }
}

void RectangleSprite::SetBorderColor(Gdiplus::Color color)
{
    if (m_borderColor.GetValue() != color.GetValue())
    {
        m_borderColor = color;
        Sprite::Invalidate();
    }
}

void RectangleSprite::OnDraw(Gdiplus::Graphics &g, const Gdiplus::RectF &rcDirty)
{
    Gdiplus::SolidBrush brush(m_color);
    Gdiplus::RectF rc = Sprite::GetRect();
    // 解决矩形左上角漏掉一个点的问题
    // 我的理解是Gdiplus认为像素在0.5那里 
    // http://stackoverflow.com/questions/3147569/pixel-behaviour-of-fillrectangle-and-drawrectangle
    rc.X = -0.5f;
    rc.Y = -0.5f;
    g.FillRectangle(&brush, rc);
    Gdiplus::Pen pen(m_borderColor);
    g.DrawRectangle(&pen, rc);
    Sprite::OnDraw(g, rcDirty);
}

LuaObject * RectangleSprite::GetLuaSide()
{
    if (!m_luaSide)
    {
        m_luaSide = new RectangleSpriteLua(this);
    }
    return m_luaSide;
}

} // namespace cs
