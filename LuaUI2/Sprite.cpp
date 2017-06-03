/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/

#include "StdAfx.h"
#include "Sprite.h"
#include "SpriteLua.h"
#include "ResourceManager.h"
#include <cmath>

extern lua_State *g_L; // GUI线程用的主lua state

namespace cs {

Sprite::Sprite(void)
{
	m_rect.X = 0;
	m_rect.Y = 0;
	m_rect.Width = 10;
	m_rect.Height = 10;

	m_hostWnd = NULL;

	m_firstChild = NULL;
	m_lastChild = NULL;
	m_prevSibling = NULL;
	m_nextSibling = NULL;
	m_parent = NULL;

	m_bVisible = true;
	m_enableFocus = false;
	m_bMouseIn = false;
	m_bClipChildren = false;
    m_bShowCaret = false;

    m_luaSide = NULL;
    m_notify = NULL;
}

Sprite::~Sprite(void)
{
	m_hostWnd = NULL;
	Sprite *sp = m_firstChild;
	while(sp)
	{
		Sprite *tmp = sp->m_nextSibling;
		sp->Unref(); // FIXME 有没有想过这里其实如果数量太多 会不会爆栈呢? 事实上是一个递归调用.
		sp = tmp;
	}
	m_firstChild = NULL;
	m_lastChild = NULL;
	m_prevSibling = NULL;
	m_nextSibling = NULL;
	m_parent = NULL;

    delete m_luaSide;
	LOG(<<"sprite deleted"); // TODO 加个名字
}

Gdiplus::RectF Sprite::GetRect()
{
	return m_rect;
}

Gdiplus::RectF Sprite::GetAbsRect()
{
	Sprite *sp = m_parent;
	Gdiplus::RectF rcSelf = GetRect();
	Gdiplus::RectF rcParent;
	while(sp)
	{
		rcParent = sp->GetRect();
		rcSelf.Offset(rcParent.X, rcParent.Y);
		sp = sp->m_parent;
	}
	return rcSelf;
}

void Sprite::SetRect( Gdiplus::RectF rect )
{
	// 检查下宽高是否小于0 是则设为0 然后0宽或0高要在OnDraw这些里面特殊处理一下
	rect.Width = max(0.0f, rect.Width);
	rect.Height = max(0.0f, rect.Height);
	if (!m_rect.Equals(rect))
	{
		Gdiplus::RectF rcOld = m_rect;
		// TODO OnMove
		Invalidate(); // 旧矩形
		m_rect = rect;
		Invalidate(); // 新矩形
		// 原先回掉的顺序错了 导致在OnSize里面GetRect和OnSize的参数不一样 诡异错误
		if (rect.Width != rcOld.Width || rect.Height != rcOld.Height)
		{
            // OnSize(rect.Width, rect.Height);
            Gdiplus::SizeF size;
            rect.GetSize(&size);
            SendNotify(eSizeChanged, &size);
		}
	}
}

void Sprite::Invalidate()
{
	// 0指针访问 不挂是因为x64系统一个bug 记得打开调试中的Win32异常断点
	HostWindow *wnd = GetHostWindow();
	if (wnd)
	{
		RECT rc;
		Gdiplus::RectF rf = GetAbsRect();
		rc.left = (LONG)(rf.GetLeft() - 0.5f);
		rc.top = (LONG)(rf.GetTop() - 0.5f);  // TODO FIXME 这个值是试出来的 不知其所以然
		rc.right = (LONG)(rf.GetRight() + 1.5f); // 缩小窗口TabCtrl会有拖影 这里改成2 就可以消除拖影现象
		rc.bottom = (LONG)(rf.GetBottom() + 1.5f); // 很诡异 可能是因为GdiPlus认为x取大的 width也取大的
		::InvalidateRect(wnd->GetHWND(), &rc, TRUE);
	}
}

// 只在CUIWindow::AddSprite中使用
void Sprite::SetHostWnd( HostWindow *wnd )
{
	LOG("BEGIN");
	// FIXME 可能有性能问题 AddChild时候跑这里 会使算法复杂度暴增
	m_hostWnd = wnd;
	// m_hostWnd->Ref(); 应该是弱引用
	//Sprite *sp = m_firstChild;
	//while(sp)
	//{
	//	sp->SetHostWnd(wnd);
	//	sp = sp->m_nextSibling;
	//}
	LOG("END");
}

void Sprite::OnDraw( Gdiplus::Graphics &g, const Gdiplus::RectF &rcDirty )
{
	if (!m_bVisible)
	{
		return; // 子节点也不会被绘制
	}
	// 前序遍历 让父节点先绘制
	//Gdiplus::RectF rc = GetRect();
	//if (10 == rc.Width && 10 == rc.Height)
	//{
	//	LOGW(<<L"Orignal Size 10 10"); // 检查下有没有多余的重绘
	//}
	if (m_bClipChildren)
	{
		Gdiplus::RectF rcClip = GetRect();
		rcClip.X = 0.0f;
		rcClip.Y = 0.0f;
		g.SetClip(rcClip);
	}
    PaintEvent ev;
    ev.graphics = &g;
    ev.rcDirty = rcDirty;
    SendNotify(ePaint, &ev);
    //this->ClientDraw(g, rcDirty);
	Sprite *sp = m_firstChild;
	while(sp)
	{
		// 如果需要重绘部分矩形和sp相交则重画它 否则不重画
		// 这里还有个问题就是 父矩形必须比子矩形要大 否则可能父的相交不到 而子的相交的到
		// 可能要强制这一原理 类似于浏览器 会撑大
		Gdiplus::RectF rc2 = sp->GetRect();
		Gdiplus::RectF rcAbs = sp->GetAbsRect();
		rcAbs.X -= 0.5f; // FIXME 有时无法得到重画导致边界1像素消失
		rcAbs.Y -= 0.5f;
		rcAbs.Width += 1.0f;
		rcAbs.Height += 1.0f;
		if (rcDirty.IntersectsWith(rcAbs))
		{
			g.TranslateTransform(rc2.X, rc2.Y);
			sp->OnDraw(g, rcDirty);
			g.TranslateTransform(-rc2.X, -rc2.Y);
		}
		sp = sp->m_nextSibling;
	}
	if (m_bClipChildren)
	{
		g.ResetClip();
	}
}

void Sprite::AddChild( Sprite *sp )
{
	if (!m_firstChild)
	{
		assert(NULL == m_lastChild);
		m_firstChild = sp;
		m_lastChild = sp;
	}
	else
	{
		Sprite *tmp = m_lastChild;
		m_lastChild = sp;
		sp->m_prevSibling = tmp;
		tmp->m_nextSibling = sp;
	}
	//sp->SetHostWnd(m_hostWnd); // 递归去设置 防止任意顺序插入导致问题 FIXME 可能有性能问题
	sp->m_parent = this;
	sp->Ref();
}

void Sprite::OnMouseEvent( lua_State *L, MouseEvent *ev )
{
	switch(ev->message)
	{
	case WM_MOUSEMOVE:
        SendNotify(eMouseMove, ev);
        //OnMouseMove(event->x, event->y, event->flag);
		if (!m_bMouseIn)
		{
			m_bMouseIn = true;
            SendNotify(eMouseEnter, ev);
			//OnMouseEnter();
		}
		break;
	case WM_MOUSEWHEEL:
		//OnMouseWheel(event);
        SendNotify(eMouseWheel, ev);
		break;
	case WM_LBUTTONDOWN:
        //OnLButtonDown(event->x, event->y, event->flag);
        SendNotify(eLBtnDown, ev);
		if (m_enableFocus)
		{
			GetHostWindow()->SetFocusSprite(this);
		}
		break;
	case WM_LBUTTONUP:
        //OnLButtonUp(event->x, event->y, event->flag);
        SendNotify(eLBtnUp, ev);
        break;
	case WM_MOUSELEAVE:
		m_bMouseIn = false;
        //OnMouseLeave();
        SendNotify(eMouseLeave, ev);
        break;
	}		
}

void Sprite::OnCapturedMouseEvent( lua_State *L, MouseEvent *ev)
{
	switch(ev->message)
	{
	case WM_MOUSEMOVE:
        SendNotify(eMouseMove, ev);
        //OnMouseMove(event->x, event->y, event->flag);
		break;
	case WM_LBUTTONDOWN:
        SendNotify(eLBtnDown, ev);
        //OnLButtonDown(event->x, event->y, event->flag);
		break;
	case WM_LBUTTONUP:
        SendNotify(eLBtnUp, ev);
        //OnLButtonUp(event->x, event->y, event->flag);
        break;
	}			
}

void Sprite::OnKeyEvent( lua_State *L, UINT message, DWORD keyCode, DWORD flag )
{
	KeyEvent ev;
    ev.keyCode = keyCode;
    ev.flag = flag;

	switch(message)
	{
	case WM_KEYDOWN:
        SendNotify(eKeyDown, &ev);
        //OnKeyDown(keyCode, flag);
		break;
	case WM_KEYUP:
        SendNotify(eKeyUp, &ev);
        //OnKeyUp(keyCode, flag);
		break;
	case WM_CHAR:
        SendNotify(eCharInput, &ev);
        //OnChar(keyCode, flag);
		break;
	}
}

void Sprite::OnImeInput(lua_State *L, LPCTSTR text)
{
    SendNotify(eImeInput, const_cast<wchar_t *>(text));
}

void Sprite::EnableFocus( bool enable )
{
	m_enableFocus = enable;
}

HostWindow * Sprite::GetHostWindow()
{
	Sprite *sp = this;
	while (sp->m_parent)
	{
		sp = sp->m_parent;
	}
	return sp->m_hostWnd;
}

void Sprite::SetCapture()
{
	assert(GetHostWindow()); // 这个在lua包装函数里检查 报成lua错误
	if (GetHostWindow())
	{
		GetHostWindow()->SetCapture(this);
	}
}

void Sprite::ReleaseCapture()
{
	assert(GetHostWindow()); // 这个在lua包装函数里检查 报成lua错误
	if (GetHostWindow())
	{
		GetHostWindow()->ReleaseCapture();
	}
}

void Sprite::BringToFront()
{
	if (m_parent)
	{
		if (this == m_parent->m_lastChild)
		{
			return;
		}
		// 
		if (m_prevSibling)
		{
			m_prevSibling->m_nextSibling = m_nextSibling;
		}
		else
		{
			m_parent->m_firstChild = m_nextSibling;
		}
		m_nextSibling->m_prevSibling = m_prevSibling; // 没有next说明是最后一个 而上面会跳过

		m_prevSibling = m_parent->m_lastChild;
		m_nextSibling = NULL;
		m_parent->m_lastChild->m_nextSibling = this;
		m_parent->m_lastChild = this;

		Invalidate();
	}
}

void Sprite::SetVisible( bool v )
{
	if (m_bVisible != v)
	{
		Invalidate();
	}
	m_bVisible = v;
}

bool Sprite::GetVisible()
{
	return m_bVisible;
}

void Sprite::SetClipChildren( bool bClip )
{
	if (m_bClipChildren != bClip)
	{
		m_bClipChildren = bClip;
		Invalidate();
	}
}

bool Sprite::GetClipChildren()
{
	return m_bClipChildren;
}



// http://blog.csdn.net/magic_feng/article/details/6618206
Sprite * Sprite::DispatchMouseEvent(MouseEvent *event)
{
	std::stack<Sprite *> stack;
	std::stack<Sprite *> reverse;
	stack.push(this);
	Sprite *sp = NULL;
	
	while(!stack.empty())
	{
		sp = stack.top();
		stack.pop();
		reverse.push(sp); // visit in reversed order.
		
		Sprite *sp2 = sp->m_lastChild;
		while (sp2)
		{
			stack.push(sp2);
			sp2 = sp2->m_prevSibling;
		}
	}
	while (!reverse.empty())
	{
		sp = reverse.top();
		reverse.pop();

		Gdiplus::RectF rc = sp->GetAbsRect();
		//LOG("HitTest:"<< rc.X << "," <<rc.Y << "," << rc.Width << "," << rc.Height);
		if (rc.Contains(event->x, event->y))
		{
			MouseEvent e2 = *event;
			e2.x -= rc.X;
			e2.y -= rc.Y;
			sp->OnMouseEvent(g_L, &e2);
		}
	}
	return NULL;
}

Sprite * Sprite::GetAncestor()
{
	Sprite *sp = this;
	while (sp->m_parent)
	{
		sp = sp->m_parent;
	}
	return sp;
}

Sprite * Sprite::GetParent()
{
	return m_parent;
}

void Sprite::TrackMouseLeave()
{
	HostWindow *wnd = GetHostWindow();
	if (wnd)
	{
		wnd->TrackMouseLeave(this);
	}
}

void Sprite::RemoveChild( Sprite *sp )
{
	assert(sp != this);
	assert(sp->m_parent == this);
	// TODO 要不要线性查找一下 是不是真的是子项目呢
#ifdef _DEBUG
	Sprite *f = m_firstChild;
	bool bFound = false;
	while(f)
	{
		if (f == sp)
		{
			bFound = true;
			break;
		}
		f = f->m_nextSibling;
	}
	assert(bFound);
#endif

	if (sp == this->m_lastChild)
	{
		this->m_lastChild = sp->m_prevSibling;
	}
	if (sp == this->m_firstChild)
	{
		this->m_firstChild = sp->m_nextSibling;
	}
	if (sp->m_prevSibling)
	{
		sp->m_prevSibling->m_nextSibling = sp->m_nextSibling;
	}
	if (sp->m_nextSibling)
	{
		sp->m_nextSibling->m_prevSibling = sp->m_prevSibling;
	}
	sp->Unref();
}

Sprite * Sprite::GetNextSprite()
{
	return m_nextSibling;
}



Sprite * Sprite::GetPrevSprite()
{
	return m_prevSibling;
}

Sprite * Sprite::GetFirstSubSprite()
{
	return m_firstChild;
}

Sprite * Sprite::GetLastSubSprite()
{
	return m_lastChild;
}

void Sprite::ShowCaret()
{
    GetHostWindow()->ShowCaret();
    m_bShowCaret = true;
}

void Sprite::SetCaretPos(Gdiplus::RectF rc)
{
    Gdiplus::RectF arc = Sprite::GetAbsRect();
    GetHostWindow()->SetImePosition(rc.X + arc.X, rc.Y + arc.Y);
    HWND hwnd = GetHostWindow()->GetHWND();
    ::DestroyCaret(); // 这里销毁重新建立 才能改变高度
    ::CreateCaret(hwnd, NULL, (int)rc.Width, (int)rc.Height); // 可以加个参数制定虚线光标(HBITMAP)1
    ::ShowCaret(hwnd);
    GetHostWindow()->SetCaretHeight(rc.Height);
    ::SetCaretPos((int)(rc.X + arc.X), (int)(rc.Y + arc.Y));
}

void Sprite::HideCaret()
{
    GetHostWindow()->HideCaret();
    m_bShowCaret = false;
}

LuaObject * Sprite::GetLuaSide()
{
    if (!m_luaSide)
    {
        m_luaSide = new SpriteLua(this);
    }
    return m_luaSide;
}

void Sprite::SendNotify(UINT idMessage, void *message)
{
    if (m_notify)
    {
        if (m_notify->OnNotify(m_id, this, idMessage, message))
            return;
    }
    GetLuaSide()->OnNotify(m_id, this, idMessage, message);
}

void Sprite::SetNotify(INotify *notify)
{
    m_notify = notify;
}

} // namespace cs