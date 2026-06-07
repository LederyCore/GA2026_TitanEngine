#pragma once
#include "SystemLocator.h"

namespace TitanEngine
{
    struct RenderItem;
    class RenderSystem;

    class IRenderable
    {
    public:
        virtual ~IRenderable();
        virtual void FillRenderItem(RenderItem& outItem) const = 0;
        virtual bool IsRenderActive() const = 0;
    };
}