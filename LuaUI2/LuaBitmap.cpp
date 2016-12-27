#include "stdafx.h"
#include "LuaBitmap.h"

LuaBitmap::LuaBitmap():
	m_bmp(NULL)
{

}

LuaBitmap::~LuaBitmap()
{
	delete m_bmp;
	m_bmp = NULL;
}

LuaBitmap* LuaBitmap::FromFile( LPCWSTR path )
{
	LuaBitmap *thiz = new LuaBitmap;
	Gdiplus::Bitmap *loadBmp = Gdiplus::Bitmap::FromFile(path);
	// 这里要拷贝一次 如果是PNG的话 一定生成PARGB格式(性能) pre-multiplied alpha channel
	if (loadBmp->GetLastStatus() != Gdiplus::Ok)
	{
		return thiz;
	}
	Gdiplus::Bitmap *newBmp= new Gdiplus::Bitmap( loadBmp->GetWidth(), loadBmp->GetHeight(),
		PixelFormat32bppPARGB );
	Gdiplus::Graphics *pGraphics= new Gdiplus::Graphics( newBmp );

	pGraphics->Clear( 0xffffffff );  // clear to solid white

	Gdiplus::Rect sizeR( 0,0,loadBmp->GetWidth(), loadBmp->GetHeight());
	pGraphics->DrawImage( loadBmp, sizeR, 0,0,
		(int)loadBmp->GetWidth(),
		(int)loadBmp->GetHeight(),
		Gdiplus::UnitPixel );
	delete pGraphics;
	delete loadBmp;

	assert(thiz->m_bmp == NULL);
	thiz->m_bmp = newBmp;
	return thiz;
}

int LuaBitmap::GetSize( lua_State *L )
{
	LuaBitmap *thiz = CheckLuaObject<LuaBitmap>(L, 1); 
	assert(thiz->m_bmp);
	lua_pushinteger(L, thiz->m_bmp->GetWidth());
	lua_pushinteger(L, thiz->m_bmp->GetHeight());
	return 2;
}

int LuaBitmap::GetFourStateInfo( lua_State *L )
{
	LuaBitmap *thiz = CheckLuaObject<LuaBitmap>(L, 1); 
	if (!thiz->m_bmp)
	{
		return 0;
	}
	int count = 0;
	int width = thiz->m_bmp->GetWidth();
	Gdiplus::Color delimiter(127, 0, 127);
	for (int i = 0; i < width && count <= 3; i ++)
	{
		Gdiplus::Color color;
		thiz->m_bmp->GetPixel(i, 0, &color);
		if (color.GetValue() == delimiter.GetValue())
		{
			lua_pushinteger(L, i);
			count ++;
		}
	}
	if (count != 3)
	{
		return 0;
	}
	return count;
}

int LuaBitmap::GetNineInOneInfo( lua_State *L )
{
	LuaBitmap *thiz = CheckLuaObject<LuaBitmap>(L, 1); 
	if (!thiz->m_bmp)
	{
		return 0;
	}
	int count = 0;
	int width = thiz->m_bmp->GetWidth();
	Gdiplus::Color delimiter(255, 0, 255);
	for (int i = 0; i < width && count < 2; i ++)
	{
		Gdiplus::Color color;
		thiz->m_bmp->GetPixel(i, 0, &color);
		if (color.GetValue() == delimiter.GetValue())
		{
			lua_pushinteger(L, i);
			count ++;
		}
	}
	if (count != 2)
	{
		return 0;
	}
	int height = thiz->m_bmp->GetHeight();
	for (int i = 0; i < height && count < 4; i ++)
	{
		Gdiplus::Color color;
		thiz->m_bmp->GetPixel(0, i, &color);
		if (color.GetValue() == delimiter.GetValue())
		{
			lua_pushinteger(L, i);
			count ++;
		}
	}
	if (count != 4)
	{
		return 0;
	}
	return count;
}

