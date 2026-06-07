#pragma once
#include <directxtk/SimpleMath.h>
#include <d2d1.h>

namespace TitanEngine
{
    struct RenderItem
    {
        ID2D1Bitmap* bitmap = nullptr;
        D2D1_COLOR_F                  color = D2D1::ColorF(D2D1::ColorF::White);
        DirectX::SimpleMath::Matrix   worldTransform = {};
        DirectX::SimpleMath::Vector2  size = {};
        float                         opacity = 1.f;
        int                           sortingLayer = 0;
        int                           orderInLayer = 0;
    };
}