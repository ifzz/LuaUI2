/*
** EditSprite.cpp Single line edit control.
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/

#include "stdafx.h"
#include "EditSprite.h"
#include "ResourceManager.h"

extern lua_State *g_L; // GUI线程用的主lua state

namespace cs {

EditSprite::EditSprite()
{
	m_font = ResourceManager::Instance()->GetFont(L"微软雅黑", 12, 0);
	//m_format.SetTrimming(Gdiplus::StringTrimmingNone);
	m_format.SetFormatFlags(Gdiplus::StringFormatFlagsMeasureTrailingSpaces | Gdiplus::StringFormatFlagsNoWrap | Gdiplus::StringFormatFlagsBypassGDI);
	m_caretCharPos = 0;
	m_selectCharPos = NO_SELCET;
	m_oldSelectCharPos = NO_SELCET;
	m_bMouseDown = false;
	m_caretScreenPos = 0.0f;
	m_showCaretCount = 0;
	m_widthSpace = 0.0f;

	Sprite::EnableFocus(true);
}

EditSprite::~EditSprite()
{

}

void EditSprite::OnImeInput( lua_State *L, LPCTSTR text )
{
	//m_text += text;
	m_text.insert(m_caretCharPos, text);
	m_caretCharPos += wcslen(text);
    Gdiplus::Graphics g(Sprite::GetHostWindow()->GetHWND());
    UpdateLayout2(g);
	AdjustScroll(L'A');// 让这个函数认为输入了一个字符
	Sprite::Invalidate();
}

void EditSprite::ClientDraw(Gdiplus::Graphics &g, const Gdiplus::RectF &rcDirty)
{
 //   Gdiplus::RectF rcClip = Sprite::GetRect();
	//rcClip.X = 0.0f;
	//rcClip.Y = 0.0f;
	//g.SetClip(rcClip); // TODO 这个要弄到基类里 开放给lua
	UpdateLayout2(g);
	m_caretScreenPos = CalcStringWidth(m_caretCharPos);
	//AdjustScroll();
	UpdateImePos(g);

	DrawStringWithSelection(g);
}

void EditSprite::DrawStringWithSelection( Gdiplus::Graphics &g )
{
	if (m_vecTextInfo.size() == 0)
	{
		return;
	}
	UINT begin = 0; // 选择的开始和结束
	UINT end = 0;
	begin = min(m_caretCharPos, m_selectCharPos);
	end = max(m_caretCharPos, m_selectCharPos);

	Gdiplus::SolidBrush brush(Gdiplus::Color(0, 0, 0));
	Gdiplus::PointF pt;
	pt = m_ptScroll;


	if (end - begin > 0 && NO_SELCET != m_selectCharPos)
	{
		for (UINT i = 0; i < begin && i < m_vecTextInfo.size(); i ++ )
		{
			wchar_t ch = m_vecTextInfo[i].ch;
			g.DrawString(&ch, 1, m_font, pt, &m_format, &brush);
			pt.X += m_vecTextInfo[i].width;
		}
		float selectWidth = 0.0f;
		for (UINT i = begin; i < end && i < m_vecTextInfo.size(); i ++ )
		{
			selectWidth += m_vecTextInfo[i].width;
		}
		Gdiplus::RectF rc(pt.X + 1.0f, pt.Y, selectWidth, m_textHeight);
		brush.SetColor(Gdiplus::Color(0, 0, 255));
		g.FillRectangle(&brush,rc);

		brush.SetColor(Gdiplus::Color(255, 255, 127));
		for (UINT i = begin; i < end && i < m_vecTextInfo.size(); i ++ )
		{
			wchar_t ch = m_vecTextInfo[i].ch;
			g.DrawString(&ch, 1, m_font, pt, &m_format, &brush);
			pt.X += m_vecTextInfo[i].width;
		}
		brush.SetColor(Gdiplus::Color(0, 0, 0));
		for (UINT i = end; i < m_vecTextInfo.size(); i ++ )
		{
			wchar_t ch = m_vecTextInfo[i].ch;
			g.DrawString(&ch, 1, m_font, pt, &m_format, &brush);
			pt.X += m_vecTextInfo[i].width;
		}
		//DrawSring(g, 0, begin, Gdiplus::Color(0, 0, 0));
		//DrawSring(g, begin, end - begin, Gdiplus::Color(0, 0, 255));
		//DrawSring(g, end, m_vecTextInfo.size() - end,  Gdiplus::Color(0, 0, 0));
	}
	else
	{
		for (UINT i = 0; i < m_vecTextInfo.size(); i ++ )
		{
			wchar_t ch = m_vecTextInfo[i].ch;
			g.DrawString(&ch, 1, m_font, pt, &m_format, &brush);
			pt.X += m_vecTextInfo[i].width;
		}
	}

}

void EditSprite::UpdateLayout2( Gdiplus::Graphics &g )
{
	m_vecTextInfo.clear();
	std::stringstream log;
	m_textWidth = 0.0f;
	m_textHeight = 0.0f;
	wstring tmp;

	Gdiplus::RectF rc, rc2;
	g.MeasureString(L" ", 1, m_font, Gdiplus::PointF(0,0), &rc);
	m_widthSpace = rc.Width;
	m_textHeight = max(m_textHeight, rc.Height);

	for (UINT i = 0; i < m_text.length(); i ++ )
	{
		TextInfo ti;
		ti.ch = m_text[i];
		if (m_text[i] != L' ')
		{
			tmp.clear();
			tmp += m_text[i];
			g.MeasureString(tmp.c_str(), 1, m_font, Gdiplus::PointF(0, 0), &rc);
			ti.width = rc.Width - m_widthSpace; //奇葩Gdiplus 实际上是给你在最后加了一个空格
		}
		else
		{
			ti.width = m_widthSpace;
		}
		m_textWidth += ti.width;
		m_textHeight = max(m_textHeight, rc.Height);
		m_vecTextInfo.push_back(ti);
		log << ti.width << ",";
	}
	m_textWidth += m_widthSpace;
	//LOG("char width:" << log.str());
}

void EditSprite::UpdateImePos( Gdiplus::Graphics &g )
{
    Gdiplus::RectF rc = Sprite::GetAbsRect();

	Gdiplus::PointF ptCaret;
	ptCaret.X = m_caretScreenPos + rc.X - 2.0f; // TODO FIXME 这个数字从哪里来的 怎么会多 呵呵
	ptCaret.Y = rc.Y;
	ptCaret = ptCaret + m_ptScroll;
	if (GetHostWindow()->GetFocusSprite() == this) 
	{
		GetHostWindow()->SetImePosition(ptCaret.X, ptCaret.Y);
		GetHostWindow()->SetCaretHeight(m_textHeight);
		HWND hwnd = GetHostWindow()->GetHWND();
		::SetCaretPos((int)ptCaret.X, (int)ptCaret.Y);
	}
}

void EditSprite::DrawSring(Gdiplus::Graphics &g, UINT pos, UINT length, Gdiplus::Color color)
{
	if (m_vecTextInfo.size() == 0)
	{
		return;
	}
	//assert(pos < m_vecTextInfo.size());
	//assert(pos + length <= m_vecTextInfo.size());

	Gdiplus::SolidBrush brush(color);
	Gdiplus::PointF pt;
	//GetRect().GetLocation(&pt);
	pt = m_ptScroll;

	// 这里有性能问题的 不该调用3次 而是应该在一个函数里完成
	// 但是比较方便 以后要是实现"语法高亮"要做一个代码编辑器的话 这个就比较有用
	for (UINT i = 0; i < pos && i < m_vecTextInfo.size(); i ++)
	{
		pt.X += m_vecTextInfo[i].width;
	}

	for (UINT i = pos; i < pos + length && i < m_vecTextInfo.size(); i ++ )
	{
		wchar_t ch = m_vecTextInfo[i].ch;
		g.DrawString(&ch, 1, m_font, pt, &m_format, &brush);
		pt.X += m_vecTextInfo[i].width;
	}
}

void EditSprite::OnKeyEvent( lua_State *L, UINT message, DWORD keyCode, DWORD flag )
{
	Sprite::OnKeyEvent(L, message, keyCode, flag);

	switch(message)
	{
	case WM_CHAR:
		switch(keyCode)
		{
		case 0x08:  // backspace
			if(m_text.length() > 0)
			{
				assert (m_caretCharPos <= m_vecTextInfo.size());
				if (m_caretCharPos > 0)
				{
					m_text.erase(m_caretCharPos - 1, 1);
					m_caretCharPos --;
                    Sprite::Invalidate();
				}
			}
			break;
		case 0x09:  // tab
		case 0x0D:  // return
		case 0x1B:  // esc
		case 0x0A:  // LF
			break;
		default:
			m_text.insert(m_caretCharPos, 1, (wchar_t)keyCode);
			m_caretCharPos ++;
			AdjustScroll(keyCode);
            Sprite::Invalidate();
			break;
		}
		break;
	case WM_KEYDOWN:
		switch(keyCode)
		{
		case VK_LEFT:
			if (m_caretCharPos > 0)
			{
				m_caretCharPos --;
			}
			AdjustScroll(keyCode);
            Sprite::Invalidate();
			break;
		case VK_RIGHT:
			if (m_caretCharPos < m_vecTextInfo.size())
			{
				m_caretCharPos ++;
			}
			AdjustScroll(keyCode);
            Sprite::Invalidate();
			break;
		case VK_HOME:
			m_caretCharPos = 0;
			AdjustScroll(keyCode);
            Sprite::Invalidate();
			break;
		case VK_END:
			m_caretCharPos = m_vecTextInfo.size();
			AdjustScroll(keyCode);
            Sprite::Invalidate();
			break;
		case VK_DELETE:
			if (m_caretCharPos < m_vecTextInfo.size())
			{
				m_text.erase(m_caretCharPos, 1);
			}
            Sprite::Invalidate();
		}
		break;
	}
	m_selectCharPos = NO_SELCET;
	m_oldSelectCharPos = NO_SELCET;
	//LOG("m_caretCharPos:"<<m_caretCharPos);
}

void EditSprite::OnSetFocus()
{
	HWND hwnd = GetHostWindow()->GetHWND();
	::ShowCaret(hwnd);
    Sprite::Invalidate();
	m_showCaretCount ++;
	//LOG("m_showCaretCount:" << m_showCaretCount);
}

void EditSprite::OnKillFocus()
{
	HWND hwnd = GetHostWindow()->GetHWND();
	::HideCaret(hwnd);
    Sprite::Invalidate();
	m_showCaretCount --;
	//LOG("m_showCaretCount:" << m_showCaretCount);
}

void EditSprite::OnMouseEvent( lua_State *L, MouseEvent *event )
{
	UINT message = event->message;
	int x = (int)event->x;
	int y = (int)event->y;
	UINT flag = event->flag;

	Sprite::OnMouseEvent(L, event);
    Gdiplus::RectF rc = Sprite::GetRect();
	rc.X = 0.0f;
	rc.Y = 0.0f;

	switch(message)
	{
	case WM_LBUTTONDOWN:
		if (rc.Contains((float)x, (float)y))
		{
			LOG("WM_LBUTTONDOWN x, y:" << x << "," << y);
			m_caretCharPos = CharPosFromPoint(x - rc.X - m_ptScroll.X, y - rc.Y - m_ptScroll.Y);
			m_selectCharPos = m_caretCharPos;
			m_oldSelectCharPos = NO_SELCET;
			m_bMouseDown = true;
			//::SetCapture(GetHostWindow()->GetHWND());// 这里不行 因为在引擎层面把消息 屏蔽了(如果不在矩形内)
            Sprite::SetCapture();
            Sprite::Invalidate();
		}
		break;
	case WM_MOUSEMOVE:
		if (m_bMouseDown)
		{
			m_caretCharPos = CharPosFromPoint(x - rc.X - m_ptScroll.X, y - rc.Y - m_ptScroll.Y);
			//LOG("WM_MOUSEMOVE x,y:" << x << "," << y);
			if (m_oldSelectCharPos != m_caretCharPos)
			{
				//LOG("WM_MOUSEMOVE m_selectCharPos:" << m_caretCharPos);
				m_oldSelectCharPos = m_caretCharPos;
                Sprite::Invalidate();
			}
		}
		break;
	case WM_LBUTTONUP:
		//if (rc.Contains((float)x, (float)y))
		//{
			LOG("WM_LBUTTONUP");
			m_bMouseDown = false;
            Sprite::ReleaseCapture();
		//}
		break;
	}
}

UINT EditSprite::CharPosFromPoint(float x, float y)
{
	//LOG("x:" << x << " y:" << y);
	Gdiplus::PointF pt(x, y);
	Gdiplus::PointF pt2;
    Sprite::GetRect().GetLocation(&pt2);
	pt = pt - pt2;
	pt.X = max(0.0f, pt.X);
	pt.Y = max(0.0f, pt.Y);

	float width = 0.0f;
	UINT i;
	for (i = 0; i < m_vecTextInfo.size(); i ++)
	{
		width += m_vecTextInfo[i].width;
		if (pt.X < width)
		{
			break;
		}
	}
	//LOG("Mouse at char:"<< i);
	return i;
}

float EditSprite::CalcStringWidth( UINT length )
{
	float width = 0.0f;
	//assert (m_caretCharPos <= m_vecTextInfo.size());
	for (UINT i = 0; i < length && i < m_vecTextInfo.size(); i ++)
	{
		width += m_vecTextInfo[i].width;
	}
	return width + m_widthSpace;
}

void EditSprite::AdjustScroll(DWORD key)
{
	// 这个里面很复杂 不要问为什么这么写 是在草稿纸上画半天 然后还乱试 试出来的
	// 这个函数只能操作 m_ptScroll.X 请不要操作别的
	m_caretScreenPos = CalcStringWidth(m_caretCharPos);
	float textWidth = CalcStringWidth(UINT(-1)); // 得到所有文字的宽度
    Gdiplus::RectF rc = Sprite::GetRect();
	float pos = m_caretScreenPos + m_ptScroll.X;
	// TODO m_caretScreenPos 总觉得这个得带上一个m_ptPadding.X 因为在纸上画的时候 发现这样比较好画
	switch(key)
	{
	case VK_LEFT:
		if ( pos < 0.0f)
		{
			m_ptScroll.X += 50.0f;
			if (m_ptScroll.X > 0.0f)
			{
				m_ptScroll.X = 0.0f;
			}
		}
		break;
	case VK_RIGHT:
		if (pos > rc.Width)
		{
			m_ptScroll.X -= 50.0f;
			if (-m_ptScroll.X > textWidth - rc.Width) //这里其实是需要文字总长度
			{
				m_ptScroll.X = -(textWidth - rc.Width);
			}
		}
		break;
	case VK_HOME:
		m_ptScroll.X = 0.0f;
		break;
	case VK_END:
		if (m_caretScreenPos - rc.Width > 0)
			m_ptScroll.X =  -(m_caretScreenPos - rc.Width);
		break;
	default:
		// 输入字符
		if (m_caretScreenPos - rc.Width > - m_ptScroll.X)
		{
			m_ptScroll.X =  -(m_caretScreenPos - rc.Width + m_widthSpace);
		}
	}
	LOG("m_ptScroll.X:" << m_ptScroll.X << " m_caretScreenPos:" << m_caretScreenPos << " rc.Width:" << rc.Width);
}

void EditSprite::OnCapturedMouseEvent( lua_State *L, MouseEvent *event)
{
	OnMouseEvent(L, event);
}

} // namespace cs
