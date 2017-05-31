/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/
#pragma once

#include "Common.h"

class Gdiplus::Image;

struct FontInfo
{
	std::wstring familyName;
	float emSize;
	int style;
};

bool operator<(const FontInfo& lhs, const FontInfo& rhs);

class ResourceManager
{
public:
	static ResourceManager *Instance();
	static void Destroy();
	Gdiplus::Image *GetImage(UINT id);
	Gdiplus::Font *GetFont(LPCTSTR familyName, float emSize, int style);

private:
	Gdiplus::Image* ImageFromResource(HMODULE hModule, UINT nResourceID, LPCTSTR szResourceType);
	ResourceManager(void);
	~ResourceManager(void);

private:
	static ResourceManager *m_inst;
	std::map<UINT, Gdiplus::Image *> m_mapResId2Image;
	std::map<FontInfo, Gdiplus::Font *> m_mapFont;
	DISALLOW_COPY_AND_ASSIGN(ResourceManager);
};
