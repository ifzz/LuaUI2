/*
** LuaHttp.cpp Asynchronse HTTP rountine 
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/

#include "StdAfx.h"
#include "LuaHttp.h"
#include "..\include\curl\curl.h"

extern HINSTANCE g_hInstance;
//extern lua_State *g_L; // 不要这样 尽量带L 不然以后做多State 会有各种问题 还有协程

namespace cs {

#define WM_HTTP_GET			(WM_APP + 1)
#define WM_HTTP_DOWNLOAD	(WM_APP + 2)
#define WM_HTTP_PROGRESS	(WM_APP + 3)

LuaHttp::LuaHttp(void)
{
	RegisterClass(); // 好像这个可以多次调用而无副作用
	m_hwnd = ::CreateWindowEx( 0, L"LuaUI2Http", L"dummy_name", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL );
	CURLcode code = curl_global_init(CURL_GLOBAL_ALL);
	code = code;
}

LuaHttp::~LuaHttp(void)
{
	curl_global_cleanup();
}

void LuaHttp::RegisterClass()
{
	WNDCLASS wc; 
	// Register the main window class. 
	wc.style = CS_HREDRAW | CS_VREDRAW; 
	wc.lpfnWndProc = (WNDPROC) WndProc; 
	wc.cbClsExtra = 0; 
	wc.cbWndExtra = 0; 
	wc.hInstance = g_hInstance; 
	wc.hIcon = NULL; // LoadIcon(NULL, IDI_APPLICATION); 
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); 
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); 
	wc.lpszMenuName =  NULL; 
	wc.lpszClassName = L"LuaUI2Http"; 

	if (!::RegisterClass(&wc)) 
	{	assert(false); }
}

int LuaHttp::Get( lua_State *L )
{
	LuaHttp *thiz = CheckLuaObject<LuaHttp>(L, 1);
	lua_remove(L, 1);
	HttpParam *param = new HttpParam;
	param->url = luaL_checkstring(L, 1);
	if (lua_isfunction(L, 2))
	{
		lua_pushvalue(L, 2);
		param->cbFinish = luaL_ref(L, LUA_REGISTRYINDEX);
	}
	param->L = L;
	param->hwnd = thiz->m_hwnd;
	::QueueUserWorkItem(GetThread, param, 0);
	return 0;
}

int LuaHttp::Post( lua_State *L )
{
	LuaHttp *thiz = CheckLuaObject<LuaHttp>(L, 1);
	lua_remove(L, 1); 
	HttpParam *param = new HttpParam;
	param->L = L;
	param->hwnd = thiz->m_hwnd;
	param->url = luaL_checkstring(L, 1);
	param->post = luaL_checkstring(L, 2);
	if (lua_isfunction(L, 3))
	{
		lua_pushvalue(L, 3);
		param->cbFinish = luaL_ref(L, LUA_REGISTRYINDEX);
	}
	::QueueUserWorkItem(GetThread, param, 0);
	return 0;
}

int LuaHttp::DownloadToFile( lua_State *L )
{
	LuaHttp *thiz = CheckLuaObject<LuaHttp>(L, 1);
	HttpParam *param = new HttpParam;
	lua_remove(L, 1); 
	param->url = luaL_checkstring(L, 1);
	param->file = luaL_checkstring(L, 2);
	param->cbFinish = LUA_NOREF;
	if (lua_isfunction(L, 3))
	{
		lua_pushvalue(L, 3);
		param->cbFinish = luaL_ref(L, LUA_REGISTRYINDEX);
	}
	if (lua_isfunction(L, 4))
	{
		lua_pushvalue(L, 4);
		param->cbProgress = luaL_ref(L, LUA_REGISTRYINDEX);
	}
	param->L = L;
	param->hwnd = thiz->m_hwnd;
	::QueueUserWorkItem(DownloadThread, param, 0);
	return 0;
}

DWORD __stdcall LuaHttp::GetThread( void * data)
{
	HttpParam *param = (HttpParam *)data;
	
	CURL *curl = curl_easy_init();
	//curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(curl, CURLOPT_URL, param->url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteToStringCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, param);
	param->curlError = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &param->responseCode);
	curl_easy_cleanup(curl);

	::PostMessage(param->hwnd, WM_HTTP_GET, (WPARAM)param, 0);
	return 0;
}

DWORD __stdcall LuaHttp::PostThread( void * data)
{
	HttpParam *param = (HttpParam *)data;

	CURL *curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, param->url.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, param->post.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteToStringCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, param);
	param->curlError = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &param->responseCode);
	curl_easy_cleanup(curl);

	::PostMessage(param->hwnd, WM_HTTP_GET, (WPARAM)param, 0);
	return 0;
}

DWORD __stdcall LuaHttp::DownloadThread( void * data)
{
	HttpParam *param = (HttpParam *)data;

	CStringW path = Utf8ToUtf16(param->file.c_str(), param->file.length());

	assert(param->hFile == INVALID_HANDLE_VALUE);
	param->hFile = CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE != param->hFile)
	{
		CURL *curl = curl_easy_init();
		param->curl = curl;
		curl_easy_setopt(curl, CURLOPT_URL, param->url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteToFileCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, param);
		param->curlError = curl_easy_perform(curl);
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &param->responseCode);
		curl_easy_cleanup(curl);
		param->curl = NULL;
		CloseHandle(param->hFile);
	}
	else
	{
		param->win32Error = ::GetLastError();
	}

	::PostMessage(param->hwnd, WM_HTTP_DOWNLOAD, (WPARAM)param, 0);
	return 0;
}

size_t LuaHttp::WriteToStringCallback( char *ptr, size_t size, size_t nmemb, void *userdata )
{
	HttpParam *param = (HttpParam *)userdata;
	if (param->cbFinish != LUA_NOREF)
	{
		param->data.append(ptr, size * nmemb); // FIXME 这里无法支持带0的二进制串? 
	}
	return size * nmemb;
}

size_t LuaHttp::WriteToFileCallback( char *ptr, size_t size, size_t nmemb, void *userdata )
{
	HttpParam *param = (HttpParam *)userdata;
	CURL *curl = param->curl;
	ProgressParam *progress = new ProgressParam;
	curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &progress->total_size);
	curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD, &progress->current_size);
	curl_easy_getinfo(curl, CURLINFO_SPEED_DOWNLOAD, &progress->speed);
	progress->cbProgress = param->cbProgress;
	progress->L = param->L;
	::PostMessage(param->hwnd, WM_HTTP_PROGRESS, (WPARAM)progress, 0);
	DWORD cbWrite = 0;
	BOOL bRet = WriteFile(param->hFile, ptr, size * nmemb, &cbWrite, NULL);
	if (!bRet)
	{
		param->win32Error = ::GetLastError();
	}
	return cbWrite;
}

LRESULT CALLBACK LuaHttp::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_CREATE:
		return 0;
	case WM_HTTP_GET:
		do 
		{
			HttpParam *param = (HttpParam *)wparam;
			if (LUA_NOREF !=  param->cbFinish)
			{
				lua_State *L = param->L;
				lua_rawgeti(L, LUA_REGISTRYINDEX, param->cbFinish);
				if (param->data.length() > 0)
				{
					lua_pushlstring(L, param->data.c_str(), param->data.length());
				}
				else
				{
					lua_pushnil(L);
				}
				lua_pushinteger(L, param->curlError);
				lua_pushinteger(L, param->responseCode);
				//luaL_show_stack(L);
				LuaPCall(L, 3, 0); // FIXME 幺蛾子 是不是应该像UI那边一样做呢? 
				// InvokeCallback才是幺蛾子 因为和正常lua 的pcall正好相反
				luaL_unref(L, LUA_REGISTRYINDEX, param->cbFinish);
			}
			delete param;
		} while (0);
		break;
	case WM_HTTP_DOWNLOAD:
		do 
		{
			HttpParam *param = (HttpParam *)wparam;
			lua_State *L = param->L;
			if (LUA_NOREF !=  param->cbFinish)
			{
				lua_rawgeti(L, LUA_REGISTRYINDEX, param->cbFinish);
				// TODO 这里加个参数表示是否成功
				lua_pushinteger(L, param->curlError);
				lua_pushinteger(L, param->win32Error);
				lua_pushinteger(L, param->responseCode);
				LuaPCall(L, 3, 0);
				luaL_unref(L, LUA_REGISTRYINDEX, param->cbFinish);
			}
			if (LUA_NOREF !=  param->cbProgress)
			{
				luaL_unref(L, LUA_REGISTRYINDEX, param->cbProgress);
			}
			delete param;
		} while (0);
		break;
	// 这里可以加一个 GetJson 返回后直接转JSON给lua 省的复制字符串到lua里去了 
	// 好像又不靠谱 因为万一解析失败 lua要把数据显示出来 可能是php那边出错了 都在c++里就不好弄了 不在乎这点性能
	case WM_HTTP_PROGRESS:
		do 
		{
			ProgressParam *progress = (ProgressParam*)wparam;
			if (LUA_NOREF !=  progress->cbProgress)
			{
				lua_State *L = progress->L;
				lua_rawgeti(L, LUA_REGISTRYINDEX, progress->cbProgress);
				lua_pushnumber(L, progress->total_size);
				lua_pushnumber(L, progress->current_size);
				lua_pushnumber(L, progress->speed);
				LuaPCall(L, 3, 0);
			}
			delete progress;
		} while (0);
	}
	return ::DefWindowProc(hwnd, msg, wparam, lparam);
}

Object * LuaHttp::GetCppSide()
{
    return NULL;
}

LuaObject * LuaHttp::GetLuaSide()
{
    return NULL;
}

} // namespace cs
