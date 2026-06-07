#pragma once
#include "IRenderable.h"
#include "RenderItem.h"
#include <vector>
#include <d2d1.h>

namespace TitanEngine
{
    class RenderSystem final
    {
    public:
        RenderSystem() = default;
        ~RenderSystem() = default;

        void Register(IRenderable* renderable);
        void Unregister(IRenderable* renderable);

        void Render(ID2D1DeviceContext* ctx);
        void Clear();

    private:
        std::vector<IRenderable*> m_renderables;
        std::vector<RenderItem>   m_renderItems;

        void Collect();
        void Sort();
        void Flush(ID2D1DeviceContext* ctx);
    };
}