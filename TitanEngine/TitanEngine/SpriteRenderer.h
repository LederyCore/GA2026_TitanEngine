#pragma once
#include "Component.h"
#include "IRenderable.h"
#include "RenderItem.h"
#include <d2d1.h>
#include <directxtk/SimpleMath.h>

namespace TitanEngine
{
    class SpriteRenderer : public ComponentBase<SpriteRenderer>
        , public IRenderable
    {
    public:
        ID2D1Bitmap* sprite = nullptr;
        DirectX::SimpleMath::Vector2 size = {};
        float                        opacity = 1.f;
        int                          sortingLayer = 0;
        int                          orderInLayer = 0;

        bool IsRenderActive()              const override;
        void FillRenderItem(RenderItem& out) const override;
    };
}