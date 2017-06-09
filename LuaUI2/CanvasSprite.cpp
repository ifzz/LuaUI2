/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/

#include "StdAfx.h"
#include "CanvasSprite.h"
#include "CanvasSpriteLua.h"
#include "LBitmap.h"

extern lua_State *g_L; // GUI线程用的主lua state

namespace cs {

CanvasSprite::CanvasSprite(Gdiplus::Graphics *g) :
	m_pGraphics(g),
	m_brush(Gdiplus::Color(255,0,255)),
	m_spaceWidth(-1.0f),
    m_luaSide(NULL)
{
	m_format.SetFormatFlags(Gdiplus::StringFormatFlagsMeasureTrailingSpaces | Gdiplus::StringFormatFlagsNoWrap | Gdiplus::StringFormatFlagsBypassGDI);
	m_font = new Gdiplus::Font(L"微软雅黑", 12.0f, 0, Gdiplus::UnitPixel, NULL);
}

CanvasSprite::~CanvasSprite(void)
{
	delete m_font;
	m_font = INVALID_POINTER(Gdiplus::Font);
    delete m_luaSide;
    m_luaSide = INVALID_POINTER(CanvasSpriteLua);
}

/*
void CanvasSprite::OnDraw( Gdiplus::Graphics &g, const Gdiplus::RectF &rcDirty )
{
    lua_State *L = g_L;
    // TODO 其实这里可以弄一个缓存位图
	DWORD startTime = ::GetTickCount();
	m_pGraphics = &g;
	if (Sprite::GetClipChildren())
	{
        Gdiplus::RectF rcClip = Sprite::GetRect();
		rcClip.X = 0.0f;
		rcClip.Y = 0.0f;
		g.SetClip(rcClip);
	}
    this->ClientDraw(rcDirty);
    if (Sprite::GetClipChildren())
	{
		g.ResetClip();
	}
	m_pGraphics = NULL;
	//LOG("time:" << ::GetTickCount() - startTime);
	Sprite::OnDraw(g, rcDirty);
}
*/

void CanvasSprite::DrawLine( Gdiplus::PointF p1, Gdiplus::PointF p2, float width )
{
	if (!m_pGraphics)
		return;
	Gdiplus::Pen pen(m_color, width); // TODO 每次都创建会不会效率低呢?
	m_pGraphics->DrawLine(&pen, p1, p2);
}

void CanvasSprite::DrawChar( Gdiplus::PointF pt, wchar_t ch)
{
	if (!m_pGraphics || !m_font)
		return;
	m_brush.SetColor(m_color);
	m_pGraphics->DrawString(&ch, 1, m_font, pt, &m_format, &m_brush);
}

void CanvasSprite::SetColor( const Gdiplus::Color &color )
{
	m_color = color;
}

bool CanvasSprite::SetFont( const wchar_t *family, float size, UINT style )
{
	delete m_font;
	m_font = new Gdiplus::Font(family, size, style, Gdiplus::UnitPixel, NULL);
	if (!(m_font->IsAvailable()))
	{
		delete m_font;
		m_font = NULL; 
		m_spaceWidth = -1;
		return false;
	}
	m_spaceWidth = -1;
	return true;
}

Gdiplus::RectF CanvasSprite::MeasureChar( wchar_t ch )
{
	Gdiplus::PointF pt(0.0f, 0.0f);
	Gdiplus::RectF rc, rc2;
	if (!m_font)
	{
		return rc2;
	}
	Gdiplus::Graphics *pTmpGraphics = NULL;
	if (!m_pGraphics)
	{
        __debugbreak();
		//pTmpGraphics = Gdiplus::Graphics::FromHWND(GetHostWindow()->GetHWND());
		m_pGraphics = pTmpGraphics;
	}
	if (m_spaceWidth < 0)
	{
		m_pGraphics->MeasureString(L" ", 1, m_font, pt, &rc);
		m_spaceWidth = rc.Width;
	}
	m_pGraphics->MeasureString(&ch, 1, m_font, pt, &rc2);
	//if (ch != L' ' && ch != 12288) 
	if (rc2.Width - m_spaceWidth > 0.0f) // unicode里面有很多种空白
	{
		rc2.Width -= m_spaceWidth;
	}
	if (pTmpGraphics)
	{
		delete pTmpGraphics;
		m_pGraphics = NULL;
	}
	return rc2;
}

//void CanvasSprite::OnImeInput( lua_State *L, LPCTSTR text )
//{
    // TODO 这个应该挪到基类 这个类将不再继承于Sprite
	//CStringA str = Utf16ToUtf8(text, wcslen(text));
	//lua_pushlstring(L, str, str.GetLength());
	//this->InvokeCallback(L, "OnImeInput", 1, 0);
//}



void CanvasSprite::DrawRect( const Gdiplus::RectF& rc )
{
	if (!m_pGraphics)
		return;
	m_brush.SetColor(m_color);
	m_pGraphics->FillRectangle(&m_brush, rc);
}

void CanvasSprite::DrawBitmap( Bitmap *bmp, Gdiplus::RectF rcSrc, Gdiplus::RectF rcDst )
{
    if (!m_pGraphics)
        return;
	m_pGraphics->DrawImage(bmp->Get(), rcDst, rcSrc.X, rcSrc.Y, rcSrc.Width, rcSrc.Height , Gdiplus::UnitPixel);
}

void CanvasSprite::DrawBitmap( Bitmap *bmp, float x , float y, float srcX, float srcY, float srcWidth, float srcHeight )
{
    if (!m_pGraphics)
        return;
	m_pGraphics->DrawImage(bmp->Get(), x, y, srcX, srcY, srcWidth, srcHeight, Gdiplus::UnitPixel);
}

void CanvasSprite::DrawBitmap( Bitmap *bmp, float x , float y )
{
    if (!m_pGraphics)
        return;
	m_pGraphics->DrawImage(bmp->Get(), x, y);
}

void CanvasSprite::MeasureString( const wchar_t *psz, Gdiplus::RectF rc, Gdiplus::RectF *rcOut )
{
	if (!m_pGraphics || !m_font)
		return ;

	Gdiplus::StringFormat format;
	Gdiplus::PointF pt(rc.X, rc.Y);

	m_pGraphics->MeasureString(psz, -1, m_font, pt, rcOut);
}

void CanvasSprite::DrawString( const wchar_t* psz, Gdiplus::RectF rc, UINT hAlign, UINT vAlign )
{
	if (!m_pGraphics || !m_font)
		return ;

	m_brush.SetColor(m_color);
	m_format.SetAlignment(static_cast<Gdiplus::StringAlignment>(hAlign));
	m_format.SetLineAlignment(static_cast<Gdiplus::StringAlignment>(vAlign));
	m_pGraphics->DrawString(psz, -1, m_font, rc, &m_format, &m_brush);
}

LuaObject* CanvasSprite::GetLuaSide()
{
    if (!m_luaSide)
    {
        m_luaSide = new CanvasSpriteLua(this);
    }
    return m_luaSide;
}

void CanvasSprite::Close()
{
    m_pGraphics = NULL;
}

} // namespace cs
