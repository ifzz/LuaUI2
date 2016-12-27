// 这个文件主要导出GdiPlus资源到lua中 包括位图,字体等.
// 在lua中再包装一次 利用弱引用表和垃圾回收来控制缓存 资源共享

#pragma once

#include "LuaObject.h"

class LuaBitmap : public LuaObject
{
public:
	LuaBitmap();
	~LuaBitmap();

	static LuaBitmap* FromFile(LPCWSTR path);

	Gdiplus::Bitmap * Get() { return m_bmp; }
	
	static int GetSize(lua_State *L);
	static int GetFourStateInfo(lua_State *L);
	static int GetNineInOneInfo(lua_State *L);

	RTTI_DECLARATIONS(LuaBitmap, LuaObject)

	BEGIN_LUA_METHOD_MAP(LuaBitmap)
		LUA_METHOD_ENTRY(GetSize)
		LUA_METHOD_ENTRY(GetFourStateInfo)
		LUA_METHOD_ENTRY(GetNineInOneInfo)
	END_LUA_METHOD_MAP()

private:
	Gdiplus::Bitmap *m_bmp;
};