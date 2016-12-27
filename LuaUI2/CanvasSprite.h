/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/

#pragma once
#include "sprite.h"

class LuaBitmap;

class CanvasSprite :
	public Sprite
{
protected:
	CanvasSprite(void);
	virtual ~CanvasSprite(void);

public:
	static void CreateInstance(CanvasSprite **ppObj);

	void OnDraw( lua_State *L, Gdiplus::Graphics &g, const Gdiplus::RectF &rcDirty );

	static int Invalidate(lua_State *L);

	void DrawLine(Gdiplus::PointF p1, Gdiplus::PointF p2, Gdiplus::Color color, float width);
	static int DrawLine(lua_State *L);

	void DrawRect(const Gdiplus::RectF& rc);
	static int DrawRect(lua_State *L);

	void DrawChar(Gdiplus::PointF pt, wchar_t ch, Gdiplus::Color);
	static int DrawChar(lua_State *L);

	Gdiplus::RectF MeasureChar(wchar_t ch);
	static int MeasureChar(lua_State *L);

	void DrawString(const wchar_t* psz, Gdiplus::RectF rc, UINT hAlign, UINT vAlign);
	static int DrawString(lua_State *L);

	static int MeasureString(lua_State *L);
	void MeasureString(const wchar_t *psz, Gdiplus::RectF rc, Gdiplus::RectF *rcOut );

	void DrawBitmap(LuaBitmap *bmp, Gdiplus::RectF rcSrc, Gdiplus::RectF rcDst);
	void DrawBitmap(LuaBitmap *bmp, float x , float y, float srcX, float srcY, float srcWidth, float srcHeight);
	void DrawBitmap(LuaBitmap *bmp, float x , float y);
	static int DrawBitmap(lua_State *L);

	void SetColor( const Gdiplus::Color &color );
	static int SetColor(lua_State *L);

	bool SetFont( const wchar_t *family, float size, UINT style);
	static int SetFont(lua_State *L);

	static int ShowCaret(lua_State *L);
	void ShowCaret();

	static int SetCaretPos(lua_State *L);
	void SetCaretPos(Gdiplus::RectF rc);

	static int HideCaret(lua_State *L);
	void HideCaret();

	virtual void OnImeInput( lua_State *L, LPCTSTR text );

	RTTI_DECLARATIONS(CanvasSprite, Sprite)

	BEGIN_LUA_METHOD_MAP(CanvasSprite)
		LUA_CHAIN_METHOD_MAP(Sprite)
		LUA_METHOD_ENTRY(DrawChar)
		LUA_METHOD_ENTRY(DrawBitmap)
		LUA_METHOD_ENTRY(DrawLine)
		LUA_METHOD_ENTRY(DrawRect)
		LUA_METHOD_ENTRY(DrawString)
		LUA_METHOD_ENTRY(HideCaret)
		LUA_METHOD_ENTRY(Invalidate)
		LUA_METHOD_ENTRY(MeasureChar)
		LUA_METHOD_ENTRY(MeasureString)
		LUA_METHOD_ENTRY(SetColor)
		LUA_METHOD_ENTRY(SetFont)
		LUA_METHOD_ENTRY(ShowCaret)
		LUA_METHOD_ENTRY(SetCaretPos)
	END_LUA_METHOD_MAP()

private:
	Gdiplus::Graphics *m_pGraphics;
	Gdiplus::StringFormat m_format;
	Gdiplus::Font *m_font;
	Gdiplus::SolidBrush m_brush;
	Gdiplus::Color m_color;
	float m_spaceWidth;
	bool m_bShowCaret;
};
