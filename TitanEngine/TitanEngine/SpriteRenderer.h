#pragma once
#include "Component.h"
#include "IRenderable.h"
#include "RenderItem.h"

namespace TitanEngine
{
    class SpriteRenderer : public ComponentBase<SpriteRenderer>
        , public IRenderable
    {
    public:
        ID2D1Bitmap* sprite = nullptr;
        DirectX::SimpleMath::Vector2  size = {};
        float                         opacity = 1.f;
        int                           sortingLayer = 0;
        int                           orderInLayer = 0;

        void FillRenderItem(RenderItem& out) const override;
    };
}