/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/

#include "StdAfx.h"
#include "CanvasSprite.h"
#include "LuaBitmap.h"

CanvasSprite::CanvasSprite(void):
	m_pGraphics(NULL),
	m_brush(Gdiplus::Color(255,0,255)),
	m_spaceWidth(-1.0f),
	m_bShowCaret(false)
{
	m_format.SetFormatFlags(Gdiplus::StringFormatFlagsMeasureTrailingSpaces | Gdiplus::StringFormatFlagsNoWrap | Gdiplus::StringFormatFlagsBypassGDI);
	m_font = new Gdiplus::Font(L"微软雅黑", 12.0f, 0, Gdiplus::UnitPixel, NULL);
}

CanvasSprite::~CanvasSprite(void)
{
	delete m_font;
	m_font = NULL;
}

void CanvasSprite::CreateInstance( CanvasSprite **ppObj )
{
	*ppObj = new CanvasSprite;
}

void CanvasSprite::OnDraw( lua_State *L, Gdiplus::Graphics &g, const Gdiplus::RectF &rcDirty )
{
	// TODO 其实这里可以弄一个缓存位图
	DWORD startTime = ::GetTickCount();
	m_pGraphics = &g;
	if (GetClipChildren())
	{
		Gdiplus::RectF rcClip = GetRect();
		rcClip.X = 0.0f;
		rcClip.Y = 0.0f;
		g.SetClip(rcClip);
	}
	RectF2LuaRect(L, rcDirty);
	InvokeCallback(L, "OnDraw", 1, 0);
	if (GetClipChildren())
	{
		g.ResetClip();
	}
	m_pGraphics = NULL;
	//LOG("time:" << ::GetTickCount() - startTime);
	Sprite::OnDraw(L, g, rcDirty);
}

int CanvasSprite::Invalidate( lua_State *L )
{
	Sprite *thiz = CheckLuaObject<Sprite>(L, 1); // 看看RTTI究竟可行否
	// TODO 这里加上指定大小
	thiz->Invalidate();
	return 0;
}

void CanvasSprite::DrawLine( Gdiplus::PointF p1, Gdiplus::PointF p2, Gdiplus::Color color, float width )
{
	if (!m_pGraphics)
		return;
	Gdiplus::Pen pen(color, width); // TODO 每次都创建会不会效率低呢?
	m_pGraphics->DrawLine(&pen, p1, p2);
}

int CanvasSprite::DrawLine( lua_State *L )
{
	CanvasSprite *thiz = CheckLuaObject<CanvasSprite>(L, 1); 
	Gdiplus::PointF p1, p2;
	p1.X = (float)luaL_checknumber(L, 2);
	p1.Y = (float)luaL_checknumber(L, 3);
	p2.X = (float)luaL_checknumber(L, 4);
	p2.Y = (float)luaL_checknumber(L, 5);
	float width = (float)luaL_checknumber(L, 6);
	thiz->DrawLine(p1, p2, thiz->m_color, width);

	return 0;
}

void CanvasSprite::DrawChar( Gdiplus::PointF pt, wchar_t ch, Gdiplus::Color color)
{
	if (!m_pGraphics || !m_font)
		return;
	m_brush.SetColor(color);
	m_pGraphics->DrawString(&ch, 1, m_font, pt, &m_format, &m_brush);
}

int CanvasSprite::DrawChar( lua_State *L )
{
	CanvasSprite *thiz = CheckLuaObject<CanvasSprite>(L, 1); 

	wchar_t ch = luaL_checkinteger(L, 2);
	Gdiplus::PointF p1;
	p1.X = (float)luaL_checknumber(L, 3);
	p1.Y = (float)luaL_checknumber(L, 4);
	thiz->DrawChar(p1, ch, thiz->m_color);
	return 0;
}

int CanvasSprite::SetColor( lua_State *L )
{
	CanvasSprite *thiz = CheckLuaObject<CanvasSprite>(L, 1); 
	BYTE r, g, b;
	r = (BYTE)luaL_checkinteger(L, 2);
	g = (BYTE)luaL_checkinteger(L, 3);
	b = (BYTE)luaL_checkinteger(L, 4);
	Gdiplus::Color color(r, g, b);
	thiz->SetColor(color);
	return 0;
}

void CanvasSprite::SetColor( const Gdiplus::Color &color )
{
	m_color = color;
}

int CanvasSprite::SetFont( lua_State *L )
{
	CanvasSprite *thiz = CheckLuaObject<CanvasSprite>(L, 1); 
	CStringW family = luaL_checkwstring(L, 2);
	float size = (float)luaL_checknumber(L, 3);
	int style = luaL_checkinteger(L, 4);
	if (!thiz->SetFont(family, size, style))
	{
		luaL_error(L, "font create failed"); // TODO 其他函数要检查m_font
	}
	return 0;
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
		pTmpGraphics = Gdiplus::Graphics::FromHWND(GetHostWindow()->GetHWND());
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

int CanvasSprite::MeasureChar( lua_State *L )
{
	CanvasSprite *thiz = CheckLuaObject<CanvasSprite>(L, 1); 
	//if (!thiz->m_pGraphics)
	//	luaL_error(L, "cannot be called outside OnDraw");
	wchar_t ch = luaL_checkinteger(L, 2);
	Gdiplus::RectF rc = thiz->MeasureChar(ch);
	lua_pushnumber(L, rc.Width);
	lua_pushnumber(L, rc.Height);
	return 2;
}

void CanvasSprite::OnImeInput( lua_State *L, LPCTSTR text )
{
	CStringA str = Utf16ToUtf8(text, wcslen(text));
	lua_pushlstring(L, str, str.GetLength());
	this->InvokeCallback(L, "OnImeInput", 1, 0);
}

int CanvasSprite::ShowCaret( lua_State *L )
{
	CanvasSprite *thiz = CheckLuaObject<CanvasSprite>(L, 1); 
	thiz->ShowCaret();
	return 0;
}

void CanvasSprite::ShowCaret()
{
	GetHostWindow()->ShowCaret();
}

// rect 指定光标的位置和大小
int CanvasSprite::SetCaretPos( lua_State *L )
{
	CanvasSprite *thiz = CheckLuaObject<CanvasSprite>(L, 1); 
	Gdiplus::RectF rc = luaL_checkrectf(L, 2);
	thiz->SetCaretPos(rc);
	return 0;
}

void CanvasSprite::SetCaretPos( Gdiplus::RectF rc )
{
	Gdiplus::RectF arc = GetAbsRect();
	GetHostWindow()->SetImePosition(rc.X + arc.X, rc.Y + arc.Y);
	HWND hwnd = GetHostWindow()->GetHWND();
	::DestroyCaret(); // 这里销毁重新建立 才能改变高度
	::CreateCaret(hwnd, NULL, (int)rc.Width, (int)rc.Height); // 可以加个参数制定虚线光标(HBITMAP)1
	::ShowCaret(hwnd);
	GetHostWindow()->SetCaretHeight(rc.Height); 
	::SetCaretPos((int)(rc.X + arc.X), (int)(rc.Y + arc.Y));
}


int CanvasSprite::HideCaret( lua_State *L )
{
	CanvasSprite *thiz = CheckLuaObject<CanvasSprite>(L, 1); 
	thiz->HideCaret();
	return 0;
}

void CanvasSprite::HideCaret()
{
	GetHostWindow()->HideCaret();
	m_bShowCaret = false;
}

int CanvasSprite::DrawRect( lua_State *L )
{
	CanvasSprite *thiz = CheckLuaObject<CanvasSprite>(L, 1); 
	Gdiplus::RectF rc = luaL_checkrectf(L, 2);
	thiz->DrawRect(rc);
	return 0;
}

void CanvasSprite::DrawRect( const Gdiplus::RectF& rc )
{
	if (!m_pGraphics)
		return;
	m_brush.SetColor(m_color);
	m_pGraphics->FillRectangle(&m_brush, rc);
}

int CanvasSprite::DrawBitmap( lua_State *L )
{
	CanvasSprite *thiz = CheckLuaObject<CanvasSprite>(L, 1);
	LuaBitmap *bmp = CheckLuaObject<LuaBitmap>(L, 2);
	if (!thiz->m_pGraphics)
		luaL_error(L, "no graphics");
	if (lua_istable(L, 3) && lua_istable(L, 4))
	{
		Gdiplus::RectF rcSrc = luaL_checkrectf(L, 3);
		Gdiplus::RectF rcDst = luaL_checkrectf(L, 4);
		thiz->DrawBitmap(bmp, rcSrc, rcDst);
	}
	else if (lua_gettop(L) == 8)
	{
		float x = (float)luaL_checknumber(L, 3);
		float y = (float)luaL_checknumber(L, 4);
		float srcX = (float)luaL_checknumber(L, 5);
		float srcY = (float)luaL_checknumber(L, 6);
		float srcWidth = (float)luaL_checknumber(L, 7);
		float srcHeight = (float)luaL_checknumber(L, 8);
		thiz->DrawBitmap(bmp, x, y, srcX, srcY, srcWidth, srcHeight);
	}
	else if (lua_gettop(L) == 4)
	{
		float x = (float)luaL_checknumber(L, 3);
		float y = (float)luaL_checknumber(L, 4);
		thiz->DrawBitmap(bmp, x, y);
	}
	else
	{
		luaL_error(L, "type error");
	}
	return 0;
}

void CanvasSprite::DrawBitmap( LuaBitmap *bmp, Gdiplus::RectF rcSrc, Gdiplus::RectF rcDst )
{
	m_pGraphics->DrawImage(bmp->Get(), rcDst, rcSrc.X, rcSrc.Y, rcSrc.Width, rcSrc.Height , Gdiplus::UnitPixel);
}

void CanvasSprite::DrawBitmap( LuaBitmap *bmp, float x , float y, float srcX, float srcY, float srcWidth, float srcHeight )
{
	m_pGraphics->DrawImage(bmp->Get(), x, y, srcX, srcY, srcWidth, srcHeight, Gdiplus::UnitPixel);
}

void CanvasSprite::DrawBitmap( LuaBitmap *bmp, float x , float y )
{
	m_pGraphics->DrawImage(bmp->Get(), x, y);
}

// TODO 这个没测
int CanvasSprite::MeasureString( lua_State *L )
{
	CanvasSprite *thiz = CheckLuaObject<CanvasSprite>(L, 1);

	CStringW str = luaL_checkwstring(L, 2);
	Gdiplus::RectF rc = luaL_checkrectf(L, 3);
	//const char* align = luaL_checkstring(L, 4);
	Gdiplus::RectF rcOut;
	thiz->MeasureString(str, rc, &rcOut);
	RectF2LuaRect(L, rcOut);
	return 1;
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

int CanvasSprite::DrawString( lua_State *L )
{
	CanvasSprite *thiz = CheckLuaObject<CanvasSprite>(L, 1);
	CStringW text = luaL_checkwstring(L, 2);
	Gdiplus::RectF rc = luaL_checkrectf(L, 3);
	UINT hAlign = 0;
	if (lua_gettop(L) >= 4)
	{
		hAlign = luaL_checkinteger(L, 4);
	}
	UINT vAlign = 0;
	if (lua_gettop(L) >= 5)
	{
		vAlign = luaL_checkinteger(L, 5);
	}
	thiz->DrawString(text, rc, hAlign, vAlign);
	return 0;
}
