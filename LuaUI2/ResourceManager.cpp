/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/

#include "StdAfx.h"
#include "ResourceManager.h"


bool operator<( const FontInfo& lhs, const FontInfo& rhs )
{
	if (lhs.familyName.compare(rhs.familyName) < 0)
	{
		return true;
	}
	else if (lhs.familyName.compare(rhs.familyName) > 0)
	{
		return false;
	}
	if (lhs.emSize < rhs.emSize)
	{
		return true;
	}
	else if (lhs.emSize > rhs.emSize)
	{
		return false;
	}
	if (lhs.style < rhs.style)
	{
		return true;
	}
	else
	{
		return false;
	}
}

ResourceManager *ResourceManager::m_inst = NULL;

ResourceManager * ResourceManager::Instance()
{
	if (!m_inst)
	{
		m_inst = new ResourceManager;
	}
	return m_inst;
}


void ResourceManager::Destroy()
{
	delete m_inst;
}


ResourceManager::ResourceManager(void)
{
}

ResourceManager::~ResourceManager(void)
{
	for (std::map<UINT, Gdiplus::Image *>::iterator iter = m_mapResId2Image.begin();
		iter != m_mapResId2Image.end(); iter++)
	{
		delete iter->second;
	}
	for (std::map<FontInfo, Gdiplus::Font *>::iterator iter = m_mapFont.begin();
		iter != m_mapFont.end(); iter++)
	{
		delete iter->second;
	}
}

Gdiplus::Image * ResourceManager::GetImage( UINT id )
{
	std::map<UINT, Gdiplus::Image *>::iterator iter = m_mapResId2Image.find(id);
	if (iter != m_mapResId2Image.end())
	{
		return iter->second;
	}
	Gdiplus::Image *img = ImageFromResource(::GetModuleHandle(NULL), id, L"IMG");
	if (img)
	{
		m_mapResId2Image[id] = img;
	}
	return img;
}

Gdiplus::Image* ResourceManager::ImageFromResource( HMODULE hModule, UINT nResourceID, LPCTSTR szResourceType )
{
	Gdiplus::Image* pImage = NULL;

	HRSRC hRes = FindResource(hModule, MAKEINTRESOURCE(nResourceID), szResourceType);
	if(hRes != NULL)
	{
		DWORD dwResSize = SizeofResource(hModule, hRes);
		if(dwResSize > 0)
		{
			HGLOBAL hResData = LoadResource(hModule, hRes);
			if(hResData != NULL)
			{
				LPVOID lpResourceData = LockResource(hResData);
				if(lpResourceData != NULL)
				{
					HGLOBAL hResourceBuffer = GlobalAlloc(GMEM_MOVEABLE, dwResSize);
					if(hResourceBuffer != NULL)
					{
						LPVOID lpResourceBuffer = GlobalLock(hResourceBuffer); 
						if(lpResourceBuffer != NULL)
						{
							CopyMemory(lpResourceBuffer, lpResourceData, dwResSize);
							GlobalUnlock(hResourceBuffer);

							IStream* piStream = NULL; 
							if(CreateStreamOnHGlobal(hResourceBuffer, TRUE, &piStream) == S_OK)
							{
								pImage = Gdiplus::Image::FromStream(piStream);
								piStream->Release(); // 交给GdiPlus来释放 CreateStreamOnHGlobal 第二个参数为TRUE
							}
						}
						//GlobalFree(hResourceBuffer);
					}
				}
				FreeResource(hResData);
			}
		}
	}
	if(pImage != NULL)
	{
		if(Gdiplus::Ok != pImage->GetLastStatus())
		{
			delete pImage;
			pImage = NULL;
		}
	}
	return pImage;
}

Gdiplus::Font * ResourceManager::GetFont( LPCTSTR familyName, float emSize, int style )
{
	FontInfo info;
	info.familyName = familyName;
	info.emSize = emSize;
	info.style = style;
	std::map<FontInfo, Gdiplus::Font *>::iterator iter = m_mapFont.find(info);
	if (iter != m_mapFont.end())
	{
		return iter->second;
	}
	Gdiplus::Font *font = new Gdiplus::Font(familyName, emSize, style, Gdiplus::UnitPixel, NULL);
	if (font->GetLastStatus() != Gdiplus::Ok)
	{
		delete font;
		font = NULL;
	}
	return font;
}
