/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/
#pragma once
#include "Common.h"
#include "Object.h"

#undef GetClassName

namespace cs {

class LuaObject
{
public:
	LuaObject(void);
	virtual ~LuaObject(void);

	virtual const char* GetClassName() = 0;

    virtual Object *GetCppSide() = 0;

	virtual void RegisterMethods(lua_State *L, int tblMethod) = 0;

	int Register( lua_State *L, const char *className );

	static int GCMethod(lua_State *L);

	static int GetCallbacks(lua_State *L);

	static int SetCallbacks(lua_State *L);

	// 把这个对象放入lua中 创建一个新的userdata包裹之 引用计数+1 lua栈+1
	void PushToLua(lua_State *L);

    virtual bool OnNotify(UINT idMessage, void *message) { return false; };

	bool InvokeCallback(lua_State *L, const char *name, int nargs, int nresult);

    lua_State *GetLuaState();
	
private:
	static void GetWeakTable( lua_State *L );
    lua_State *m_L;
	int m_refUserData;
    DISALLOW_COPY_AND_ASSIGN(LuaObject);
};

#define BEGIN_LUA_METHOD_MAP(x)  virtual const char *GetClassName() override {return #x;} \
virtual void RegisterMethods( lua_State *L, int metatable) override {

#define LUA_METHOD_ENTRY(x) 	lua_pushcfunction(L, x); lua_setfield(L, metatable, #x);

#define END_LUA_METHOD_MAP() }

#define LUA_CHAIN_METHOD_MAP(parentType) parentType::RegisterMethods(L, metatable);

template<class T>
T* CheckLuaObject( lua_State *L, int idx)
{
    LuaObject **ppObj = (LuaObject **)lua_touserdata(L, idx);
	if (ppObj)
	{
        LuaObject* obj = *ppObj;
		assert(obj);
        T* t = dynamic_cast<T*>(obj);
        if (t)
			return t;
	}
	luaL_error(L, "C object type checking failed: #%d", idx);
	return NULL;
}

} // namespace cs
