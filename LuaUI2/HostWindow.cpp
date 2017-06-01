/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/

#include "StdAfx.h"
#include "HostWindow.h"
#include "HostWindowLua.h"
#include "Common.h"
#include "Sprite.h"


extern HINSTANCE g_hInstance;
extern lua_State *g_L; // GUI线程用的主lua state

namespace cs {

HostWindow::HostWindow(void)
{
	m_style = WS_OVERLAPPEDWINDOW|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
	m_exStyle = 0;

	m_rect.X = 0;
	m_rect.Y = 0;
	m_rect.Width = 800;
	m_rect.Height = 600;

	m_hwndParent = ::GetDesktopWindow();
	m_hwnd = NULL;
	m_sprite = NULL;

	m_rectComposition.left = 0;
	m_rectComposition.top = 0;
	m_rectComposition.right = 5;
	m_rectComposition.bottom = 20;

	m_spFocus = NULL;
	m_spCapture = NULL;
	m_spHover = NULL;

	m_caretHeight = 20;
}

HostWindow::~HostWindow(void)
{
	if (m_sprite)
	{
		m_sprite->Unref();
	}
	m_hwnd = NULL;
	m_sprite = NULL;
	m_spFocus = NULL;
    delete m_luaSide;
    m_luaSide = NULL;
}

void HostWindow::Create(HostWindow *parent, Gdiplus::RectF rc, DWORD style, DWORD exStyle)
{
    HWND hParent = NULL;
    if (!parent)
    {
        hParent = ::GetDesktopWindow();
    }
    else
    {
        hParent = parent->m_hwnd;
    }
    //KObject *obj = dynamic_cast<KObject*>(this);
    //assert(obj != NULL);
    ::CreateWindowEx(exStyle, L"LuaUI2", L"LuaUI2", style,
        (int)rc.X, (int)rc.Y, (int)rc.Width, (int)rc.Height,
        hParent, NULL, g_hInstance, this);
}

void HostWindow::SetRect(Gdiplus::RectF rc)
{
    ::MoveWindow(m_hwnd, (int)rc.X, (int)rc.Y, (int)rc.Width, (int)rc.Height, TRUE);
}

// TODO 记得窗口过程 里面一定要先push HWND message wparam lparam 到lua堆栈里
// 然后再 pcall 一个cfunction 再去解析参数 不然一旦lua报错 就找不到错误在哪 而且程序会挂掉

void HostWindow::RegisterWndClass()
{
	WNDCLASS wc;
	wc.style         = 0; // CS_DBLCLKS;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = g_hInstance;
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;// (HBRUSH)(COLOR_WINDOW + 1); // TODO 改成NULL 防止系统去擦除背景(闪烁) 双缓冲 还有clip children clip sibling
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = L"LuaUI2";

	RegisterClass(&wc);
}

LRESULT CALLBACK HostWindow::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	LuaStackCheck check(g_L);


	// TODO 这里少WM_NCDESTROY
	lua_State *L = g_L;
	lua_pushcfunction(L, LuaHandleMessage);
	lua_pushlightuserdata(L, hwnd);
	lua_pushnumber(L, msg);
	lua_pushlightuserdata(L, (void*)wparam);
	lua_pushlightuserdata(L, (void*)lparam);
	// 这里一定要保护起来 因为这个是所有函数的入口 除了最初启动 最初启动有lua开发环境做pcall
	if (LuaPCall(L, 4, 1))
	{
		if(lua_isnumber(L, -1)) //不检查会挂掉
		{
			LRESULT ret = lua_tointeger(L, -1);
			lua_pop(L, 1);
			return ret;
		}
		else // 返回nil表示没处理这个消息
		{
			lua_pop(L, 1); // nil也要pop
			return ::DefWindowProc(hwnd, msg, wparam, lparam);
		}
	}
	return 0;
}

void HostWindow::OnMouseEvent(lua_State *L, UINT message, WPARAM wparam, LPARAM lparam)
{
	MouseEvent event;
	event.message = message;
	event.flag = LOWORD(wparam);

	if (WM_MOUSEWHEEL == message)
	{
		POINT pt;
		pt.x = (short)LOWORD(lparam);
		pt.y = (short)HIWORD(lparam);
		::ScreenToClient(GetHWND(), &pt);
		event.x = (float)pt.x;
		event.y = (float)pt.y;
		event.delta = (float)(short)HIWORD(wparam) / 120.0F;
	}
	else
	{
		event.x = (float)(short)LOWORD(lparam); //先切2字节 然后转有符号 再转浮点
		event.y = (float)(short)HIWORD(lparam);
		event.delta = 0.0f;
	}

	if (m_spCapture)
	{
		Gdiplus::RectF rc = m_spCapture->GetAbsRect();
		event.x -= rc.X;
		event.y -= rc.Y;
		m_spCapture->OnCapturedMouseEvent(L, &event);
	}
	else if (m_sprite)
	{
		//if (WM_LBUTTONDOWN == message)
		//{
			m_sprite->DispatchMouseEvent(&event);
			
			std::vector<Sprite *> defer_remove;
			for (std::tr1::unordered_set<Sprite *>::iterator iter = m_setTrackMouseLeave.begin();
				iter != m_setTrackMouseLeave.end(); ++iter)
			{
				Sprite *sp = *iter;
				Gdiplus::RectF rc = sp->GetAbsRect();
				if (!rc.Contains(event.x, event.y))
				{
					MouseEvent e2 = event;
					e2.message = WM_MOUSELEAVE;
					sp->OnMouseEvent(L, &e2);
					defer_remove.push_back(sp);
					// Fire the event and remove sp from the set;
				}
			}
			for (std::vector<Sprite *>::iterator iter = defer_remove.begin();
				iter != defer_remove.end(); ++iter)
			{
				Sprite *sp = *iter;
				m_setTrackMouseLeave.erase(sp);
			}
		//}
	}
}

int HostWindow::LuaHandleMessage( lua_State *L )
{
	HWND hwnd =  (HWND)lua_touserdata(L, 1);
	UINT message = (UINT)lua_tointeger(L, 2);
	WPARAM wparam = (WPARAM)lua_touserdata(L, 3);
	LPARAM lparam = (LPARAM)lua_touserdata(L, 4);

	HostWindow *thiz;
	if (WM_NCCREATE == message)
	{
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lparam);
        thiz = reinterpret_cast<HostWindow*>(lpcs->lpCreateParams);
		thiz->m_hwnd = hwnd;
		SetWindowLongPtr(hwnd, GWLP_USERDATA,
			reinterpret_cast<LPARAM>(thiz));
	}
	else if (WM_GETMINMAXINFO == message)
	{
		return 0; // return nil
	} else {
        thiz = reinterpret_cast<HostWindow *>
            (GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}
	assert(NULL != thiz);

    bool bHandled = false;

	switch(message)
	{
	case WM_PAINT:
		thiz->OnPaint(hwnd);
		break;
	case WM_SYNCPAINT:
		// 上面有窗口拖动的时候 会有1像素宽度和1像素高度的区域 不停的发 而WM_PAINT又是低优先级的
		// 导致有的消息被丢弃了 所以才会有白条
		// 解决在win7 没开毛玻璃 设置窗口拖动时显示内容 窗口上面的窗口被拖动 画面会花
		::InvalidateRect(hwnd, NULL, TRUE);
		//LOG("WM_SYNCPAINT");
		break;
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MOUSEWHEEL:
		thiz->OnMouseEvent(L, message, wparam, lparam);
		break;
	case WM_ERASEBKGND:
		lua_pushinteger(L, TRUE);
		return 1;
	case WM_SIZE:
        //thiz->OnSize((float)(short)LOWORD(lparam), (float)(short)HIWORD(lparam), wparam);
        thiz->m_luaSide->HandleMessage(hwnd, message, wparam, lparam, bHandled);
		lua_pushinteger(L, 0);
		return 1;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
		if (thiz->m_spFocus)
		{
			thiz->m_spFocus->OnKeyEvent(L, message, (DWORD)wparam, (DWORD)lparam);
		}
		break;
	case WM_IME_SETCONTEXT:
	case WM_IME_STARTCOMPOSITION:
	case WM_IME_COMPOSITION:
	case WM_IME_ENDCOMPOSITION:
	case WM_INPUTLANGCHANGE:
		do 
		{
			LRESULT ret = thiz->OnImeEvent(L, message, wparam, lparam);
			lua_pushinteger(L, ret);
		} while (0);
		return 1;
	case WM_SETFOCUS:
		LOG("WM_SETFOCUS");
		::CreateCaret(hwnd, NULL, 1, thiz->m_caretHeight);
		//::ShowCaret(hwnd);
		if (thiz->m_spFocus)
		{
			//thiz->m_spFocus->OnSetFocus();
            thiz->m_spFocus->SendNotify(Sprite::eSetFocus, NULL);
		}
		lua_pushinteger(L, 0);
		return 1;
	case WM_KILLFOCUS:
		LOG("WM_KILLFOCUS");
		::DestroyCaret();
		if (thiz->m_spFocus)
		{
			//thiz->m_spFocus->OnKillFocus();
            thiz->m_spFocus->SendNotify(Sprite::eKillFocus, NULL);
        }
		lua_pushinteger(L, 0);
		return 1;
	case WM_CREATE:
		do 
		{
			RECT rc;
			::GetClientRect(hwnd, &rc);
			::PostMessage(hwnd, WM_SIZE, 0, MAKELPARAM(rc.right, rc.bottom)); // TODO 这里如果创建出来的是全屏的 第2个参数怎么办呢
		} while (0);
		lua_pushinteger(L, 0);
		return 1;
	case WM_CLOSE:
		break;
	case WM_DESTROY:
        //thiz->OnDestroy();
        thiz->m_luaSide->HandleMessage(hwnd, message, wparam, lparam, bHandled);
		lua_pushinteger(L, 0);
		return 1;
	}
	//::DefWindowProc(hwnd, message, wparam, lparam);
	// bHandled ::DefWindowProc();
	lua_pushnil(L);
	return 1;
}

void HostWindow::SetGdipMode(Gdiplus::Graphics &g)
{
	g.SetCompositingMode( Gdiplus::CompositingModeSourceOver );
	g.SetCompositingQuality( Gdiplus::CompositingQualityHighSpeed );
	g.SetSmoothingMode( Gdiplus::SmoothingModeNone );
	g.SetPixelOffsetMode( Gdiplus::PixelOffsetModeHalf );
	g.SetInterpolationMode( Gdiplus::InterpolationModeNearestNeighbor );
}

void HostWindow::OnPaint(HWND hwnd )
{
	DWORD startTime = ::GetTickCount();
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	//LOG("PAINTSTRUCT:" << ps.fErase << "," << ps.fIncUpdate << "," << ps.fRestore);

	int x = ps.rcPaint.left;
	int y = ps.rcPaint.top;
	int width = ps.rcPaint.right - ps.rcPaint.left;
	int height = ps.rcPaint.bottom - ps.rcPaint.top;
	Gdiplus::RectF rcDirty((float)x, (float) y, (float)width, (float)height);

	// http://www.codeproject.com/Tips/66909/Rendering-fast-with-GDI-What-to-do-and-what-not-to
	Gdiplus::Graphics gScreen(hwnd, FALSE);
	SetGdipMode(gScreen);
	Gdiplus::Bitmap bmp(width, height, PixelFormat32bppPARGB);
	Gdiplus::Graphics gDoubleBuffer(&bmp);
	SetGdipMode(gDoubleBuffer);
	//Gdiplus::Graphics gDoubleBuffer(dcMem);
	gDoubleBuffer.TranslateTransform(-(float)x, -(float)y);
	if (m_sprite)
	{
		Gdiplus::RectF rc = m_sprite->GetRect();
		gDoubleBuffer.TranslateTransform(rc.X, rc.Y);
		m_sprite->OnDraw(gDoubleBuffer, rcDirty);
		gDoubleBuffer.TranslateTransform(-rc.X, -rc.Y);
	}
	do 
	{
		// 显示FPS
		/*
		gDoubleBuffer.TranslateTransform(rcDirty.X, rcDirty.Y);
		CStringW speed;
		speed.Format(L"%dms", ::GetTickCount() - startTime);
		Gdiplus::Font font(L"微软雅黑", 12.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
		Gdiplus::StringFormat format;
		format.SetFormatFlags(Gdiplus::StringFormatFlagsMeasureTrailingSpaces | Gdiplus::StringFormatFlagsNoWrap | Gdiplus::StringFormatFlagsBypassGDI);
		Gdiplus::SolidBrush brush(Gdiplus::Color(255,0,255));
		Gdiplus::PointF pt(0.0f, 0.0f);
		gDoubleBuffer.DrawString(speed, speed.GetLength(), &font, pt, &format, &brush);
		*/
	} while (0);
	do 
	{
		// 测试的时候显示脏矩形
		/*
		gDoubleBuffer.ResetTransform();
		Gdiplus::Pen pen(Gdiplus::Color(255,0,255), 1.0f);
		gDoubleBuffer.DrawRectangle(&pen, 0, 0, width - 1, height - 1);
		const int size = 10;
		gDoubleBuffer.DrawLine(&pen, width / 2 - size, height / 2, width / 2 + size, height / 2);
		gDoubleBuffer.DrawLine(&pen, width / 2, height / 2 - size, width / 2, height / 2 + size);
		*/
	} while (0);

	gScreen.DrawImage(&bmp,x, y);
	EndPaint(hwnd, &ps);
}

HWND HostWindow::GetHWND()
{
	return m_hwnd;
}

void HostWindow::AttachSprite( Sprite *sp )
{
	if (m_sprite)
	{
		m_sprite->Unref();
	}
	m_sprite = sp;
	sp->SetHostWnd(this);
	sp->Ref();
}

LRESULT HostWindow::OnImeEvent( lua_State *L, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    // TODO 加个开关 能够关闭输入法 当焦点不在输入框上的时候就关闭
	switch(uMsg)
	{
	case WM_IME_SETCONTEXT:
		do 
		{
			LOG(<<"WM_IME_SETCONTEXT");
			BOOL handled = FALSE;
			m_ime.CreateImeWindow(GetHWND());
			m_ime.CleanupComposition(GetHWND());
			m_ime.SetImeWindowStyle(GetHWND(), uMsg, wParam, lParam, &handled);
		} while (0);
		return 0;
	case WM_IME_STARTCOMPOSITION:
		LOG(<<"WM_IME_STARTCOMPOSITION");
		m_ime.CreateImeWindow(GetHWND());
		m_ime.ResetComposition(GetHWND());
		return 0;
	case WM_IME_COMPOSITION:
		do
		{
			LOG(<<"WM_IME_COMPOSITION");
			ImeComposition comp;
			m_ime.UpdateImeWindow(GetHWND());
			m_ime.GetResult(GetHWND(), lParam, &comp);
			if (GCS_RESULTSTR == comp.string_type)
			{
				wstring tmp = comp.ime_string; // 这里结尾居然多出个0 可能是改了谷歌的代码导致的问题
				tmp.resize(wcslen(tmp.c_str()));
				LOGW(<<tmp);
				OnImeInput(L, tmp.c_str());
				//m_text += tmp;
				//::InvalidateRect(GetHWND(), NULL, TRUE);
			}
			m_ime.ResetComposition(GetHWND());
			m_ime.EnableIME(GetHWND(), m_rectComposition, false); // 输入窗口跟随
		}while(0);
		return 0;
	case WM_IME_ENDCOMPOSITION:
		LOG(<<"WM_IME_ENDCOMPOSITION");
		m_ime.ResetComposition(GetHWND());
		m_ime.DestroyImeWindow(GetHWND());
		//::ShowCaret(m_hwnd);
		::DefWindowProc(GetHWND(), uMsg, wParam, lParam);
		return 0;
	case WM_INPUTLANGCHANGE:
		LOG(<<"WM_INPUTLANGCHANGE");
		m_ime.SetInputLanguage();
		::DefWindowProc(GetHWND(), uMsg, wParam, lParam);
		return 0;
	case WM_KEYDOWN:
		return 0;
	}
	return 0;
}

void HostWindow::SetImePosition( float x, float y )
{
	m_rectComposition.left = (int)x;
	m_rectComposition.right = (int)x + 5;
	m_rectComposition.top = (int)y;
	m_rectComposition.bottom = (int)y + 20;
}

void HostWindow::SetFocusSprite( Sprite *sp )
{
	if (m_spFocus == sp)
	{
		return;
	}
	if (m_spFocus)
	{
		//m_spFocus->OnKillFocus();
        m_spFocus->SendNotify(Sprite::eKillFocus, NULL);
	}
	m_spFocus = sp;
	if (m_spFocus)
	{
		//m_spFocus->OnSetFocus();
        m_spFocus->SendNotify(Sprite::eSetFocus, NULL);
	}
}

Sprite *HostWindow::GetFocusSprite()
{
	return m_spFocus;
}

void HostWindow::OnImeInput( lua_State *L, PCTSTR text )
{
	if (m_spFocus)
	{
		// 其他的Sprite也有可能去接受ime消息。比如再来一个RichEdit
		m_spFocus->OnImeInput(L, text);
	}
}

void HostWindow::SetCaretHeight( float h)
{
	m_caretHeight = (int)h;
}

void HostWindow::SetCapture( Sprite *sp )
{
	assert(sp->GetHostWindow() == this);
	m_spCapture = sp;
	::SetCapture(m_hwnd);
}

void HostWindow::ReleaseCapture()
{
	m_spCapture = NULL;
	::ReleaseCapture();
}

void HostWindow::ShowCaret()
{
	LOG(" ");
	::ShowCaret(m_hwnd);
}

void HostWindow::HideCaret()
{
	LOG(" ");
	::HideCaret(m_hwnd);
}

void HostWindow::TrackMouseLeave( Sprite *sp )
{
	if (sp->GetHostWindow() == this)
	{
		m_setTrackMouseLeave.insert(sp);
	}
	// TODO Track for the HWND
}

LuaObject * HostWindow::GetLuaSide()
{
    if (!m_luaSide)
    {
        m_luaSide = new HostWindowLua(this);
    }
    return m_luaSide;
}

} // namespace cs