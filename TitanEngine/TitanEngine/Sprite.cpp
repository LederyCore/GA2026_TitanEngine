#include "pch.h"
#include "Sprite.h"
#include "Texture2D.h"

namespace TitanEngine
{
    ID2D1Bitmap1* Sprite::GetBitmap() const
    {
        return texture ? texture->GetBitmap() : nullptr;
    }

    UINT Sprite::GetWidth() const
    {
        return texture ? texture->GetWidth() : 0;
    }

    UINT Sprite::GetHeight() const
    {
        return texture ? texture->GetHeight() : 0;
    }
}
