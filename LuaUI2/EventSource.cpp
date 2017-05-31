/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/


#include "StdAfx.h"
#include "EventSource.h"

// 这些方法实际上应该放到 LuaObject里面
// 想了很多 这个的问题是不好找代码
// 如果你在多处AttatchListener那你就不好找出全部的Listener 因为lua是没有"查找所有引用"的...
// 

EventSource::EventSource(lua_State *L) :
	m_L (L)
{
}

EventSource::~EventSource(void)
{
	for (EventMapType::iterator iter = m_mapEvents.begin(); iter != m_mapEvents.end(); iter ++)
	{
		for (std::list<int>::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2 ++)
		{
			luaL_unref(m_L, LUA_REGISTRYINDEX, *iter2);
		}
	}
}

void EventSource::RegisterEvent(const char* name)
{
	std::string strName(name);
	if (m_mapEvents.find(strName) == m_mapEvents.end())
	{
		m_mapEvents[strName] = std::list<int>();
	}
}

void EventSource::FireEvent( lua_State *L, const char* name, int nargs)
{
	EventMapType::iterator iter = m_mapEvents.find(std::string(name));
	if (iter != m_mapEvents.end())
	{
		for (std::list<int>::iterator iter2 = iter->second.begin();
			iter2 != iter->second.end(); iter2 ++)
		{
			// PushToLua
			int ref = (*iter2);
			lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
			// FIXME 这里不对 call了就清除栈了 
			lua_call(L, nargs, 1); // TODO 想办法把sender给带上
			if (lua_isstring(L, -1))
			{
				const char *szRet = lua_tostring(L, -1);
				// 终止传播
				if (strcmp(szRet, "abort") == 0)
				{
					break;
				}
			}
		}
	}
}

void EventSource::AttachListener( lua_State *L, const char* name, int idx, bool bFront)
{
	if (!lua_isfunction(L, idx))
	{
		luaL_error(L, "AttachListener need a function.");
	}
	EventMapType::iterator iter = m_mapEvents.find(std::string(name));
	if (iter != m_mapEvents.end())
	{
		lua_pushvalue(L, idx);
		int ref = luaL_ref(L, LUA_REGISTRYINDEX);
		std::list<int>::iterator iter2;
		if (bFront)
		{
			iter2 = iter->second.begin();
		}
		else
		{
			iter2 = iter->second.end();
		}
		iter->second.insert(iter2, ref);
	}
}
