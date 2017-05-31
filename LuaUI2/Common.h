#pragma once

/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/

// 析构时自动平衡lua堆栈
class LuaStackCheck
{
public:
	explicit LuaStackCheck( lua_State *L )
	{
		m_L = L;
		m_top = lua_gettop(L);
	}
	~LuaStackCheck()
	{
		 lua_State *L = m_L;
		if (lua_gettop(L) > m_top)
		{
			assert(false);
			//int top = lua_gettop(L);
			lua_settop(L, m_top);
		}
	}
	int SetReturn(int n)
	{
		m_top += n;
		return n;
	}

private:
	 lua_State *m_L;
	int m_top;
};

CStringW Utf8ToUtf16(LPCSTR strA, int len = -1);
CStringA Utf16ToUtf8(LPCTSTR strW, int len);

bool LuaPCall(lua_State *L, int nargs, int nresults);
void LuaLog(LPCSTR strLogA);

int LuaTraceBack (lua_State *L);
static void DebugOutputLine(CString line);
void LuaShowStack(lua_State *L);

// TODO 把这些都改成大写的 因为这是lua的namespace不能写在里面 以防止以后lua更新之后名字冲突
CString luaL_checkwstring(lua_State *L, int index);
void luaL_pushwstring(lua_State *L, LPCTSTR psz, int len = -1);
Gdiplus::Color luaL_checkcolor(lua_State *L, int index);
Gdiplus::RectF luaL_checkrectf(lua_State *L, int index);
void RectF2LuaRect( lua_State *L, const Gdiplus::RectF &rc );

#define LOG(msg) do\
{\
	std::stringstream ss;\
	ss << __FUNCTION__ << "() " msg << std::endl;\
	OutputDebugStringA(ss.str().c_str());\
} while (0)

#define LOGW(msg) do\
{\
	std::wstringstream ss;\
	ss << __FUNCTIONW__ << L"() " msg << std::endl;\
	OutputDebugString(ss.str().c_str());\
} while (0)

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&);             \
	void operator=(const TypeName&)