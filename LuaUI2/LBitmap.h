#pragma once

#include "Common.h"
#include "Object.h"

namespace cs {

class LuaObject;
class BitmapLua;

class Bitmap : public Object
{
public:
    Bitmap();
    virtual ~Bitmap();

    bool LoadFromFile(LPCWSTR path);
    Gdiplus::Bitmap * Get() { return m_bmp; }
    Gdiplus::SizeF GetSize();
    // TODO 这2个结果存起来 然后写一个c++的 9-patch.
    void GetFourStateInfo(std::vector<int> &info);
    void GetNineInOneInfo(std::vector<int> &info);

    virtual LuaObject * GetLuaSide() override;

private:
    Gdiplus::Bitmap *m_bmp;
    BitmapLua *m_luaSide;
    DISALLOW_COPY_AND_ASSIGN(Bitmap);
};

} // namespace cs
