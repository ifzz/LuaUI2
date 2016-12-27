/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/
#pragma once
#include "Common.h"
#include "KObject.h"

// 改成非模版类 用多态就可以搞定这一切 不然和KObject不兼容

class LuaObject : public KObject
{
public:
	LuaObject(void) : m_refUserData(LUA_NOREF) {}
	~LuaObject(void){}

	RTTI_DECLARATIONS(LuaObject, KObject);

	virtual const char* GetClassName() = 0;

	virtual void RegisterMethods(lua_State *L, int tblMethod) = 0;

	int Register( lua_State *L, const char *className );

	static LuaObject* CheckKObject(lua_State *L, int idx = 1, size_t typeId = TypeIdClass(), const char* typeName = NULL);

	static int GCMethod(lua_State *L);

	static int GetCallbacks(lua_State *L);

	static int SetCallbacks(lua_State *L);

	// 把这个对象放入lua中 创建一个新的userdata包裹之 引用计数+1 lua栈+1
	void PushToLua(lua_State *L);

	// nargs 参数个数 不包括this TODO 这个要测
	//void CallLuaMethod(lua_State *L, const char *method, int nargs, int nresult);

	// 设置回调函数 1 名称 2 函数(lambda)
	// static int SetCallback(lua_State *L);

	bool InvokeCallback(lua_State *L, const char *name, int nargs, int nresult);
	
private:
	static void GetWeakTable( lua_State *L );

	//int m_refEventTable;
	int m_refUserData;
};

#define BEGIN_LUA_METHOD_MAP(x)  virtual const char *GetClassName() {return #x;} \
virtual void RegisterMethods( lua_State *L, int metatable) {

#define LUA_METHOD_ENTRY(x) 	lua_pushcfunction(L, x); lua_setfield(L, metatable, #x);

#define END_LUA_METHOD_MAP() }

#define LUA_CHAIN_METHOD_MAP(parentType) parentType::RegisterMethods(L, metatable);

template<class T>
T* CheckLuaObject( lua_State *L, int idx)
{
	KObject **ppObj = (KObject **)lua_touserdata(L, idx);
	if (ppObj)
	{
		KObject* obj = *ppObj;
		assert(obj);
		if (obj->IsValid() && obj->Is(T::TypeIdClass()))
		{
			return (T*)obj;
		}
	}
	luaL_error(L, "C object type checking failed: #%d is not a %s", idx, T::TypeName().c_str());
	return NULL;
}