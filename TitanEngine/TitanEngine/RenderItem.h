#pragma once
#include <directxtk/SimpleMath.h>
#include <d2d1.h>

namespace TitanEngine
{
    struct RenderItem
    {
        ID2D1Bitmap* bitmap = nullptr;
        DirectX::SimpleMath::Matrix   worldTransform = {};
        DirectX::SimpleMath::Vector2  size = {};
        float                         opacity = 1.f;
        int                           sortingLayer = 0;
        int                           orderInLayer = 0;
    };
}