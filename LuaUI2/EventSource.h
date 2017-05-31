/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/

#pragma once
#include "LuaObject.h"
#include <map>
#include <list>

typedef std::map<std::string, std::list<int> > EventMapType;

class EventSource
{
public:
	EventSource(lua_State *L);
	~EventSource();

	void RegisterEvent(const char* name);
	void FireEvent(lua_State *L, const char* name, int nargs);
	void AttachListener(lua_State *L, const char* name, int idx, bool bFront);

private:
	EventMapType m_mapEvents;
	lua_State *m_L;
};
