/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/
#pragma once

class LuaWeakRegistry
{
public:
	LuaWeakRegistry(void);
	~LuaWeakRegistry(void);

	// 初始化 只能调用一次
	static void Create(lua_State *L);

	// 将栈顶元素放入弱引用表 并弹出栈顶元素
	static void Put(lua_State *L, void *key);

	// 根据key得到lua对象 栈顶+1
	static void Get(lua_State *L, void *key);

private:
	static bool m_bInit;
};
