/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/

#include "stdafx.h"
#include "Common.h"
#include "UniConversion.h"


bool LuaPCall(lua_State *L, int nargs, int nresults)
{
	lua_pushcfunction(L, LuaTraceBack);
	lua_insert(L, lua_gettop(L) - nargs - 1);
	int error = lua_pcall(L, nargs, nresults, lua_gettop(L) - nargs - 1);
	if (error == 0) {
		//lua_pop(L, 1);
		lua_remove(L, lua_gettop(L) - nresults);
	}
	else {
		assert(lua_isstring(L, -1));
		const char *pszError = lua_tostring(L, -1);//luaL_checkstring(L, -1);
		int msg = lua_gettop(L); // msg
		// 3种错误输出的选择 选那个?
		lua_getglobal(L, "print"); // msg, print
		if (lua_isfunction(L, -1))
		{
			lua_pushvalue(L,msg); // msg, print, msg
			lua_call(L, 1, 0); // msg
		}
		LuaLog(pszError);
#ifdef _DEBUG
		::PostQuitMessage(1); // 多么的简单粗暴 不用去关程序了 直接挂掉
#else
		// 弹框的给黑盒测试
		// TODO 要弄个开关 防止发布版弹框
		CStringW strError = Utf8ToUtf16(pszError);
		::MessageBox(NULL, strError, NULL, 0);
#endif
		lua_pop(L, 2); // stack trace string and the LuaTraceBack function
	}
	//LuaCheckStack(L);
	return error == 0;
}

// http://stackoverflow.com/questions/12256455/print-stacktrace-from-c-code-with-embedded-lua
int LuaTraceBack (lua_State *L) {
	if (!lua_isstring(L, 1))  /* 'message' not a string? */
		return 1;  /* keep it intact */
	// lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	lua_getglobal(L, "debug");
	if (!lua_istable(L, -1)) {
		lua_pop(L, 1);
		return 1;
	}
	lua_getfield(L, -1, "traceback");
	if (!lua_isfunction(L, -1)) {
		lua_pop(L, 2);
		return 1;
	}
	lua_pushvalue(L, 1);  /* pass error message */
	lua_pushinteger(L, 2);  /* skip this function and traceback */
	lua_call(L, 2, 1);  /* call debug.traceback */
	return 1;
}

void LuaLog(LPCSTR strLogA)
{
	// 判断是否打开LOG
	static int log = -1;
	if (log < 0)
	{
		log = ::PathFileExists(L"C:\\luaui_log");
		// CsDebugA("<LuaUI> PathFileExists:%d\r\n", log);
	}
	if (log == 0)
	{
		return;
	}
	// OutPutDebugStringA 需要gbk ListView需要utf16 而lua里面是utf8 sqlite还有各种开源库还强制使用utf8
	CString logW = Utf8ToUtf16(strLogA, strlen(strLogA)); // 这个是utf8转utf16
	CString line;
	int start = 0;
	int end = logW.Find(L'\n', start);
	//bool bMultiLine = false;

	while (end != -1)
	{
		//bMultiLine = true;
		line = logW.Mid(start, end - start);
		DebugOutputLine(line);
		start = end + 1;
		end = logW.Find(L'\n', start);
	}
	line = logW.Mid(start);
	DebugOutputLine(line);

	//if (bMultiLine)
	//{
	//	DebugOutputLine(L"----------");
	//}

	// 由于性能原因 不做滚动 有一个定时器 1秒滚动一次 灵感来自 eclipse logcat 
}

static void DebugOutputLine(CString line)
{
	line.Replace(L"\t", L"  ");
	line.Insert(0, L"<LuaUI> ");
	line.Append(L"\r\n");
	::OutputDebugStringW(line);
	// line.Replace(L"\r\n", L"\\r\\n"); // 下面函数已分过多行
}

CStringW Utf8ToUtf16(LPCSTR strA, int len)
{
	if(len < 0)
	{
		len = strlen(strA);
	}
	CStringW strW;
	int lenW = UTF16Length(strA, len);
	wchar_t *pbuff = strW.GetBuffer(lenW + 1);
	int ret = UTF16FromUTF8(strA, len, pbuff, lenW);
	strW.ReleaseBuffer(ret);
	return strW;
}

CStringA Utf16ToUtf8(LPCTSTR strW, int len)
{
	CStringA strA;
	int lenA = UTF8Length(strW, len);
	char *pbuff = strA.GetBuffer(lenA + 1);
	UTF8FromUTF16(strW, len, pbuff, lenA);
	strA.ReleaseBuffer(lenA);
	return strA;
}

// 网上说有点不安全 希望能找到更好的版本 比如gtk内部带的  TODO fltk里面有 (2016-08-23)
///Returns -1 if string is valid. Invalid character is put to ch.
int GetInvalidUtf8SymbolPosition(const char *input, char &ch) {
	int                 nb = 0, na = 0;
	const char *c = input;

	for (c = input; *c; c += (nb + 1)) {
		if (!(*c & 0x80))
			nb = 0;
		else if ((*c & 0xc0) == 0x80)
		{
			ch = *c;
			return (int)c - (int)input;
		}
		else if ((*c & 0xe0) == 0xc0)
			nb = 1;
		else if ((*c & 0xf0) == 0xe0)
			nb = 2;
		else if ((*c & 0xf8) == 0xf0)
			nb = 3;
		else if ((*c & 0xfc) == 0xf8)
			nb = 4;
		else if ((*c & 0xfe) == 0xfc)
			nb = 5;
		na = nb;
		while (na-- > 0)
			if ((*(c + nb) & 0xc0) != 0x80)
			{
				ch = *(c + nb);
				return (int)(c + nb) - (int)input;
			}
	}

	return -1;
}

// 如果这个函数调用之后 再check XX的话会导致内存泄漏 因为lua用longjump 栈上对象没有析构
// 貌似lua可以配置成用c++exception处理错误 2014-10-30 1625 发现lua和c++一起编译的话是用异常
CString LuaCheckWString(lua_State *L, int index)
{
	size_t len;
	const char *pText = luaL_checklstring(L, index, &len);
	char ivc;
	if (GetInvalidUtf8SymbolPosition(pText, ivc) == -1)
		return Utf8ToUtf16(pText, len);
	else
		return L"invalide utf8 string";
}

void LuaPushWString(lua_State *L, LPCTSTR psz, int len /* = -1 */)
{
	if (len < 0)
	{
		len = wcslen(psz);
	}
	CStringA strA = Utf16ToUtf8(psz, len);
	lua_pushlstring(L, strA, strA.GetLength());
}

Gdiplus::Color LuaCheckColor(lua_State *L, int index)
{
	double r = luaL_checknumber(L, index + 0);
	double g = luaL_checknumber(L, index + 1);
	double b = luaL_checknumber(L, index + 2);
	return Gdiplus::Color ((BYTE) r, (BYTE) g, (BYTE) b);
}

void LuaShowStack(lua_State *L)
{
	const int COUNT = 10;
	CStringA msg;
	msg.AppendFormat("(top:%d) ", lua_gettop(L));
	for(int i = lua_gettop(L); i >= 1 && i >= lua_gettop(L) - COUNT ; i --) {
		switch(lua_type(L, i)) {
			case LUA_TSTRING:
				msg.AppendFormat("(%s:%s) ", lua_typename(L, lua_type(L, i)), lua_tostring(L, i));
				break;
			case LUA_TNUMBER:
				msg.AppendFormat("(%s:%f) ", lua_typename(L, lua_type(L, i)), lua_tonumber(L, i));
				break;
			default:
				msg.AppendFormat("(%s:%p) ", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
		}
	}
	OutputDebugStringA(msg);
}

Gdiplus::RectF LuaCheckRectF( lua_State *L, int index )
{
	Gdiplus::RectF rc;
	luaL_checktype(L, index, LUA_TTABLE);
	lua_getfield(L, index, "x");
	if (lua_isnumber(L, -1))
	{
		rc.X = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
	}
	else
	{
		luaL_error(L, "#%d.x is not a number.", index);
	}
	lua_getfield(L, index, "y");
	if (lua_isnumber(L, -1))
	{
		rc.Y = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
	}
	else
	{
		luaL_error(L, "#%d.y is not a number.", index);
	}

	lua_getfield(L, index, "w");
	if (lua_isnumber(L, -1))
	{
		rc.Width = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
	}
	else
	{
		luaL_error(L, "#%d.w is not a number.", index);
	}
	lua_getfield(L, index, "h");
	if (lua_isnumber(L, -1))
	{
		rc.Height = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
	}
	else
	{
		luaL_error(L, "#%d.h is not a number.", index);
	}
	return rc;
}

void RectF2LuaRect( lua_State *L, const Gdiplus::RectF &rc )
{
	lua_newtable(L);
	int tRect = lua_gettop(L);
	lua_pushnumber(L, rc.X);
	lua_setfield(L, tRect, "x");
	lua_pushnumber(L, rc.Y);
	lua_setfield(L, tRect, "y");
	lua_pushnumber(L, rc.Width);
	lua_setfield(L, tRect, "w");
	lua_pushnumber(L, rc.Height);
	lua_setfield(L, tRect, "h");
}
