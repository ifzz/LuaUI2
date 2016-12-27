/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <stack>

using std::wstring;
using std::wstringstream;
using std::stringstream;

#include <lua/lua.hpp>

#include <new> // 我擦 没这个 placement new 就不能编译
#include <cassert>
#include <atlstr.h>

#include <GdiPlus.h>
#include <ShlObj.h>
//#include <vld.h> //好像有这个ZeroBrane就出故障 出不来界面

