#pragma once
#include "IRenderable.h"
#include "RenderItem.h"
#include <vector>
#include <d2d1.h>

namespace TitanEngine
{
    // RenderSystem.h
    class RenderSystem final
    {
    public:
        void Register(IRenderable* renderable);
        void Unregister(IRenderable* renderable);

        // 소팅 레이어 변경 시 호출
        void MarkSortDirty() { m_sortDirty = true; }

        void Render(ID2D1DeviceContext* ctx);
        void Clear();

    private:
        std::vector<IRenderable*> m_renderables;
        std::vector<RenderItem>   m_renderItems;
        bool                      m_sortDirty = true; // 최초 1회 정렬

        void Collect();
        void Sort();
        void Flush(ID2D1DeviceContext* ctx);
    };
}