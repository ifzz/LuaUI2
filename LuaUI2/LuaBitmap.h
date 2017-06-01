// 这个文件主要导出GdiPlus资源到lua中 包括位图,字体等.
// 在lua中再包装一次 利用弱引用表和垃圾回收来控制缓存 资源共享

#pragma once

#include "LuaObject.h"

namespace cs {

class Bitmap;

class BitmapLua : public LuaObject
{
public:
	BitmapLua(Bitmap *bmp);
	~BitmapLua();

    static int LoadFromFile(lua_State *L);
	static int GetSize(lua_State *L);
	static int GetFourStateInfo(lua_State *L);
	static int GetNineInOneInfo(lua_State *L);

	BEGIN_LUA_METHOD_MAP(Bitmap)
        LUA_METHOD_ENTRY(LoadFromFile)
		LUA_METHOD_ENTRY(GetSize)
		LUA_METHOD_ENTRY(GetFourStateInfo)
		LUA_METHOD_ENTRY(GetNineInOneInfo)
	END_LUA_METHOD_MAP()

    virtual Object * GetCppSide() override;

private:
    Bitmap *m_bmp;
    DISALLOW_COPY_AND_ASSIGN(BitmapLua);
};

} // namespace cs
