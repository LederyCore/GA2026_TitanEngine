#pragma once

namespace TitanEngine
{
    struct RenderItem;

    class IRenderable
    {
    public:
        virtual ~IRenderable() = default;
        virtual void FillRenderItem(RenderItem& outItem) const = 0;
        virtual bool IsRenderActive() const = 0;
    };
}