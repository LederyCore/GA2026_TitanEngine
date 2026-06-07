#include "pch.h"
#include "RenderSystem.h"
#include <algorithm>

namespace TitanEngine
{
    void RenderSystem::Register(IRenderable* renderable)
    {
        m_renderables.push_back(renderable);
    }

    void RenderSystem::Unregister(IRenderable* renderable)
    {
        m_renderables.erase(
            std::remove(m_renderables.begin(), m_renderables.end(), renderable),
            m_renderables.end());
    }

    void RenderSystem::Render(ID2D1DeviceContext* ctx)
    {
        if (!ctx) return;
        Collect();
        Sort();
        Flush(ctx);
        m_renderItems.clear();
    }

    void RenderSystem::Clear()
    {
        m_renderables.clear();
        m_renderItems.clear();
    }

    void RenderSystem::Collect()
    {
        m_renderItems.reserve(m_renderables.size());
        for (auto* r : m_renderables)
        {
            if (!r->IsRenderActive()) continue;

            RenderItem item;
            r->FillRenderItem(item);
            if (item.bitmap)
                m_renderItems.push_back(item);
        }
    }

    void RenderSystem::Sort()
    {
        std::sort(m_renderItems.begin(), m_renderItems.end(),
            [](const RenderItem& a, const RenderItem& b)
            {
                if (a.sortingLayer != b.sortingLayer)
                    return a.sortingLayer < b.sortingLayer;
                return a.orderInLayer < b.orderInLayer;
            });
    }

    void RenderSystem::Flush(ID2D1DeviceContext* ctx)
    {
        for (auto& item : m_renderItems)
        {
            D2D1_MATRIX_3X2_F d2dMatrix = D2D1::Matrix3x2F(
                item.worldTransform._11, item.worldTransform._12,
                item.worldTransform._21, item.worldTransform._22,
                item.worldTransform._41, item.worldTransform._42
            );

            ctx->SetTransform(d2dMatrix);
            ctx->DrawBitmap(
                item.bitmap,
                D2D1::RectF(0.f, 0.f, item.size.x, item.size.y),
                item.opacity
            );
        }

        ctx->SetTransform(D2D1::Matrix3x2F::Identity());
    }
}