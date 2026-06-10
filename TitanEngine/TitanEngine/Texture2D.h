#pragma once
#include "Resource.h"
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace TitanEngine
{
    class Texture2D : public Resource
    {
    public:
        bool Load(ID2D1DeviceContext7* ctx, const std::filesystem::path& path) override;

        ID2D1Bitmap1* GetBitmap() const { return m_bitmap.Get(); }
        UINT GetWidth()  const { return m_width; }
        UINT GetHeight() const { return m_height; }

    private:
        ComPtr<ID2D1Bitmap1> m_bitmap;
        UINT m_width  = 0;
        UINT m_height = 0;
    };
}
