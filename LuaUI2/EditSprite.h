/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/

#pragma once

#include "Sprite.h"

namespace cs {

class EditSprite : public Sprite
{
protected:
	virtual ~EditSprite();
public:
    EditSprite();

	virtual void OnImeInput( lua_State *L, LPCTSTR text);

	virtual void ClientDraw( Gdiplus::Graphics &g, const Gdiplus::RectF &rcDirty );

	virtual void OnMouseEvent( lua_State *L, MouseEvent *event );

	virtual void OnCapturedMouseEvent( lua_State *L, MouseEvent *event );

	virtual void OnKeyEvent( lua_State *L, UINT message, DWORD keyCode, DWORD flag );

	virtual void OnSetFocus();

	virtual void OnKillFocus();

	BEGIN_LUA_METHOD_MAP(EditSprite)
		//LUA_METHOD_ENTRY(GetText)
		LUA_CHAIN_METHOD_MAP(SpriteLua)
	END_LUA_METHOD_MAP()

private:
	struct TextInfo
	{
		wchar_t ch;
		float width;
	};
	struct LineInfo
	{
		std::vector<TextInfo> vecText;
	};
	enum {
		NO_SELCET = -1
	};
	void UpdateLayout2( Gdiplus::Graphics &g );
	void UpdateImePos( Gdiplus::Graphics &g );
	void DrawSring(Gdiplus::Graphics &g, UINT pos, UINT length, Gdiplus::Color color);
	UINT CharPosFromPoint(float x, float y);
	float CalcStringWidth(UINT length);
	void AdjustScroll(DWORD key);
	void DrawStringWithSelection( Gdiplus::Graphics &g );

private:
	std::wstring m_text;
	Gdiplus::Font *m_font;
	float m_textHeight;
	float m_textWidth;
	Gdiplus::StringFormat m_format;
	Gdiplus::PointF m_ptScroll; // Sprite基类的scroll不好用 还是打算控件自己管理吧 因为有边框绘制 还有滚动条 都要避开全面的滚动 并不效率 而且还要有动态创建显示对象 这些东东
	UINT m_caretCharPos;
	UINT m_selectCharPos; // 这个和上面那个组成一个选择区域
	UINT m_oldSelectCharPos;
	bool m_bMouseDown;
	float m_caretScreenPos;
	int m_showCaretCount;
	float m_widthSpace;

	std::vector<LineInfo> m_vecLineInfo; //多行用这个
	std::vector<TextInfo> m_vecTextInfo; //单行用这个
};

} // namespace cs
