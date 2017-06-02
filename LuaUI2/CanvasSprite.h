/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/

#pragma once
#include "Object.h"

namespace cs {

class Bitmap;

class CanvasSpriteLua;

class CanvasSprite :
	public Object
{
protected:
	virtual ~CanvasSprite();

public:
    CanvasSprite(Gdiplus::Graphics *g);

    virtual void ClientDraw(const Gdiplus::RectF &rcDirty) {}

	void DrawLine(Gdiplus::PointF p1, Gdiplus::PointF p2, float width);

	void DrawRect(const Gdiplus::RectF& rc);

	void DrawChar(Gdiplus::PointF pt, wchar_t ch);

	Gdiplus::RectF MeasureChar(wchar_t ch);

	void DrawString(const wchar_t* psz, Gdiplus::RectF rc, UINT hAlign, UINT vAlign);

	void MeasureString(const wchar_t *psz, Gdiplus::RectF rc, Gdiplus::RectF *rcOut );

    void DrawBitmap(Bitmap *bmp, Gdiplus::RectF rcSrc, Gdiplus::RectF rcDst);
    void DrawBitmap(Bitmap *bmp, float x, float y, float srcX, float srcY, float srcWidth, float srcHeight);
    void DrawBitmap(Bitmap *bmp, float x, float y);

	void SetColor( const Gdiplus::Color &color );

	bool SetFont( const wchar_t *family, float size, UINT style);

	//virtual void OnImeInput( lua_State *L, LPCTSTR text ) override;

    LuaObject *GetLuaSide();

private:
	Gdiplus::Graphics *m_pGraphics;
	Gdiplus::StringFormat m_format;
	Gdiplus::Font *m_font;
	Gdiplus::SolidBrush m_brush;
	Gdiplus::Color m_color;
	float m_spaceWidth;
    CanvasSpriteLua *m_luaSide;
};

} // namespace cs
