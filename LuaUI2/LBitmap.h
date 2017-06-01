#pragma once

#include "Object.h"

namespace cs {

class Bitmap : public virtual Object
{
public:
    Bitmap();
    virtual ~Bitmap();

    bool LoadFromFile(LPCWSTR path);
    Gdiplus::Bitmap * Get() { return m_bmp; }
    Gdiplus::SizeF GetSize();
    void GetFourStateInfo(std::vector<int> &info);
    void GetNineInOneInfo(std::vector<int> &info);

private:
    Gdiplus::Bitmap *m_bmp;
};

} // namespace cs
