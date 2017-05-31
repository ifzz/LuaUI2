/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/

#include "StdAfx.h"
#include "LuaObject.h"

extern lua_State *g_L; // GUI线程用的主lua state

namespace cs {

LuaObject::LuaObject(void) : 
    m_refUserData(LUA_NOREF),
    m_L(NULL)
{

}

LuaObject::~LuaObject(void)
{
    if (LUA_NOREF != m_refUserData)
    {
        lua_State *L = g_L;
        GetWeakTable(L);
        luaL_unref(L, -1, m_refUserData);
        LOG(<< "luaL_unref");
    }
}

int LuaObject::Register( lua_State *L, const char *className )
{
	LuaStackCheck check(L);

	int ret = luaL_newmetatable(L, className);
	assert(ret);
	int metatable = lua_gettop(L); // local metatable = {}
	lua_newtable(L);
	int methods = lua_gettop(L); // local methods = {}

	lua_pushvalue(L, methods);
	lua_setfield(L, metatable, "__index");  // metatable.__index = methods

	lua_pushcfunction(L, GCMethod);
	lua_setfield(L, metatable, "__gc"); // metatable.__gc = GCMethod

	lua_pushvalue(L, methods);
	lua_setfield(L, metatable, "__metatable"); // 防止lua里篡改

	lua_pushcfunction(L, GetCallbacks);
	lua_setfield(L, methods, "GetCallbacks");

	lua_pushcfunction(L, SetCallbacks);
	lua_setfield(L, methods, "SetCallbacks"); 
    // TODO 这里能不能发出一个消息呢? 或打印一点 用来调试脚本逻辑问题 比如替换了对象的消息响应函数

	RegisterMethods(L, methods);
	// 注册方法

	lua_getglobal(L, "luaui2");
	if (LUA_TTABLE == lua_type(L, -1))
	{
		lua_pushvalue(L, methods);
		lua_setfield(L, -2, GetClassName()); // 设置成全局这样lua也可以添加方法在上面
	}
	lua_pop(L, 1); // for luaui2
	lua_pop(L, 1); // for metatable
	lua_pop(L, 1); // for methods

	return check.SetReturn(0);
}

void LuaObject::PushToLua( lua_State *L )
{
	LuaStackCheck check(L);
	GetCppSide()->Ref();
    Object ** ppThis = (Object **)lua_newuserdata(L, sizeof(Object *));
	*ppThis = dynamic_cast<Object*>(this);
    assert(*ppThis != NULL);
	int udata = lua_gettop(L);

	// 检查下是不是已经注册过了
	luaL_getmetatable(L, GetClassName());
	if(!lua_istable(L, -1))
	{
		lua_pop(L, 1); // pop nil
		Register(L, GetClassName());
		luaL_getmetatable(L, GetClassName());
	}
	lua_setmetatable(L, udata); // udata
	//int type = lua_type(L, lua_gettop(L));
	check.SetReturn(1);
}

int LuaObject::GCMethod( lua_State *L )
{
	LuaObject *thiz = CheckLuaObject<LuaObject>(L, 1);
	assert(thiz);
	thiz->GetCppSide()->Unref();
	return 0;
}

int LuaObject::GetCallbacks( lua_State *L )
{
	LuaObject *thiz = CheckLuaObject<LuaObject>(L, 1);
	if (LUA_NOREF == thiz->m_refUserData)
	{
		lua_pushnil(L);
		return 1;
	}
	GetWeakTable(L);
	int ref_table = lua_gettop(L);
	lua_rawgeti(L, ref_table, thiz->m_refUserData);
	return 1;
}

int LuaObject::SetCallbacks( lua_State *L )
{
	LuaObject *thiz = CheckLuaObject<LuaObject>(L, 1);
	luaL_checktype(L, 2, LUA_TTABLE);
	GetWeakTable(L);
	int ref_table = lua_gettop(L);
	if (LUA_NOREF != thiz->m_refUserData)
	{
		luaL_unref(L, ref_table, thiz->m_refUserData);
	}
	lua_pushvalue(L, 2);
	thiz->m_refUserData = luaL_ref(L, ref_table);
	lua_pop(L, 1); // for ref_table
	return 0;
}

// TODO 考虑能不能改成"多播"的方式 就是多个callback table都是弱引用 放vector里面.
// 这样好处是c++里面就不用处理回调名称的字符串了,相对高效一点.
bool LuaObject::InvokeCallback( lua_State *L, const char *name, int nargs, int nresult )
{
	if (LUA_NOREF == m_refUserData)
	{
		goto failed;
	}
	GetWeakTable(L); // ref_table
	lua_rawgeti(L, -1, m_refUserData);
	int tblMessage = lua_gettop(L); // ref_table, tblMessage
	lua_remove(L, -2); // for ref_table
	if(!lua_istable(L, -1))
	{
		goto failed;
	}
	lua_getfield(L, -1, name);      // tblMessage, callback
	if (lua_isfunction(L, -1))
	{
		lua_remove(L, -2);              // callback
		lua_insert(L, lua_gettop(L) - nargs);
		lua_call(L, nargs, nresult);
		//LuaPCall(L, nargs, nresult);
	}
	else
	{
		lua_pop(L, 2); // pop nil, tblMessage
		goto failed;
	}
	return true;

failed:
	while (nresult > 0)
	{
		lua_pushnil(L);
		nresult --;
	}
	return false;
}

void LuaObject::GetWeakTable( lua_State *L )
{
	lua_pushliteral(L, "LuaObjectWeakRef");
	lua_rawget(L, LUA_REGISTRYINDEX);
	if (lua_istable(L, -1))
	{
		return;
	}
	else
	{
		lua_newtable(L); // 1 local t = {}
		int ref_table = lua_gettop(L);
		lua_pushvalue(L, ref_table);     // 2
		lua_setmetatable(L, ref_table); // 1 setmetatable(t, t);
		lua_pushliteral(L, "__mode");  // 2
		lua_pushliteral(L, "v");     // 3
		lua_rawset(L, ref_table);   // 1 t.__mode = "v"

		lua_pushliteral(L, "LuaObjectWeakRef"); // 2
		lua_pushvalue(L, ref_table); // 3
		lua_rawset(L, LUA_REGISTRYINDEX); //1
	}
}

lua_State * LuaObject::GetLuaState()
{
    return m_L;
}

} // namespace cs
