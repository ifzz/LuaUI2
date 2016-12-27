/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/

#include "StdAfx.h"
#include "LuaWeakRegistry.h"

LuaWeakRegistry::LuaWeakRegistry(void)
{
}

LuaWeakRegistry::~LuaWeakRegistry(void)
{
}

void LuaWeakRegistry::Create( lua_State *L )
{
	assert(m_bInit == false);
	// http://news.qq.com/a/20160329/043023.htm?pgv_ref=aio2015&ptlang=2052
	// Push our key for the global registry for our weak table (so we can fetch the table later)
	lua_pushliteral(L, "LuaWeakRegistry");

	// Now create (push) our weak table
	lua_newtable(L);
	lua_pushvalue(L, -1);  // table is its own metatable
	lua_setmetatable(L, -2);
	lua_pushliteral(L, "__mode");
	lua_pushliteral(L, "kv"); // make values weak, I don't think lightuserdata is strong ref'd so 'k' is optional.
	lua_settable(L, -3);   // metatable.__mode = "v"

	// Now that we've created a new table, put it in the global registry
	lua_settable(L, LUA_REGISTRYINDEX); /* registry[LUACOCOA_OBJECT_GLOBAL_WEAK_TABLE_ID] = weak_table */

	// hmmm...I kind of expected needing a lua_pop(L, 1); here to balance the stack, but it's already balanced.
	m_bInit = true;
}

void LuaWeakRegistry::Put( lua_State *L, void *key )
{
	int value = lua_gettop(L);
	lua_getfield(L, LUA_REGISTRYINDEX, "LuaWeakRegistry");
	assert(lua_istable(L, -1));
	lua_pushlightuserdata(L, key);
	lua_pushvalue(L, value);
	lua_settable(L, -3);
	lua_pop(L, 1); // LuaWeakRegistry
	lua_pop(L, 1); // lua value to store
}

void LuaWeakRegistry::Get( lua_State *L, void *key )
{
	int value = lua_gettop(L);
	lua_getfield(L, LUA_REGISTRYINDEX, "LuaWeakRegistry");
	assert(lua_istable(L, -1));
	lua_pushlightuserdata(L, key);
	lua_gettable(L, -2);
	lua_pop(L, 1); // LuaWeakRegistry
}

bool LuaWeakRegistry::m_bInit = false;
