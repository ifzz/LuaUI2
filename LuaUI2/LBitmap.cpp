#include "stdafx.h"
#include "LBitmap.h"

namespace cs {

LBitmap::LBitmap() :
    m_bmp(NULL)
{
}


LBitmap::~LBitmap()
{
    delete m_bmp;
    m_bmp = NULL;
}

bool LBitmap::LoadFromFile(LPCWSTR path)
{
    Gdiplus::Bitmap *loadBmp = Gdiplus::Bitmap::FromFile(path);
    // 这里要拷贝一次 如果是PNG的话 一定生成PARGB格式(性能) pre-multiplied alpha channel
    if (loadBmp->GetLastStatus() != Gdiplus::Ok)
    {
        return false;
    }
    Gdiplus::Bitmap *newBmp = new Gdiplus::Bitmap(loadBmp->GetWidth(), loadBmp->GetHeight(),
        PixelFormat32bppPARGB);
    Gdiplus::Graphics *pGraphics = new Gdiplus::Graphics(newBmp);

    pGraphics->Clear(0xffffffff);  // clear to solid white

    Gdiplus::Rect sizeR(0, 0, loadBmp->GetWidth(), loadBmp->GetHeight());
    pGraphics->DrawImage(loadBmp, sizeR, 0, 0,
        (int)loadBmp->GetWidth(),
        (int)loadBmp->GetHeight(),
        Gdiplus::UnitPixel);
    delete pGraphics;
    delete loadBmp;

    delete m_bmp;
    m_bmp = newBmp;
    return true;
}

Gdiplus::SizeF LBitmap::GetSize()
{
    assert(m_bmp);
    return Gdiplus::SizeF((float)m_bmp->GetWidth(), (float)m_bmp->GetHeight());
}

void LBitmap::GetFourStateInfo(std::vector<int> &info)
{
    info.clear();
    if (!m_bmp)
    {
        return;
    }
    int count = 0;
    int width = m_bmp->GetWidth();
    Gdiplus::Color delimiter(127, 0, 127);
    for (int i = 0; i < width && count <= 3; i++)
    {
        Gdiplus::Color color;
        m_bmp->GetPixel(i, 0, &color);
        if (color.GetValue() == delimiter.GetValue())
        {
            info.push_back(i);
            count++;
        }
    }
    if (count != 3)
    {
        info.clear();
    }
}

void LBitmap::GetNineInOneInfo(std::vector<int> &info)
{
    info.clear();
    if (!m_bmp)
    {
        return;
    }
    int count = 0;
    int width = m_bmp->GetWidth();
    Gdiplus::Color delimiter(255, 0, 255);
    for (int i = 0; i < width && count < 2; i++)
    {
        Gdiplus::Color color;
        m_bmp->GetPixel(i, 0, &color);
        if (color.GetValue() == delimiter.GetValue())
        {
            info.push_back(i);
            count++;
        }
    }
    if (count != 2)
    {
        info.clear();
        return;
    }
    int height = m_bmp->GetHeight();
    for (int i = 0; i < height && count < 4; i++)
    {
        Gdiplus::Color color;
        m_bmp->GetPixel(0, i, &color);
        if (color.GetValue() == delimiter.GetValue())
        {
            info.push_back(i);
            count++;
        }
    }
    if (count != 4)
    {
        info.clear();
    }
}

} // namespace cs
