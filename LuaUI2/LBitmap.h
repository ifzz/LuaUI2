#pragma once

#include "Object.h"

namespace cs {

class LBitmap : public virtual Object
{
public:
    LBitmap();
    virtual ~LBitmap();

    bool LoadFromFile(LPCWSTR path);
    Gdiplus::Bitmap * Get() { return m_bmp; }
    Gdiplus::SizeF GetSize();
    void GetFourStateInfo(std::vector<int> &info);
    void GetNineInOneInfo(std::vector<int> &info);

private:
    Gdiplus::Bitmap *m_bmp;
};

} // namespace cs
