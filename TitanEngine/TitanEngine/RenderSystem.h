#pragma once
#include "IRenderable.h"
#include "RenderItem.h"
#include <vector>
#include <algorithm>
#include <d2d1.h>

namespace TitanEngine
{
    class RenderSystem final
    {
    public:
        RenderSystem() = default;
        ~RenderSystem() = default;

        // ── 등록 / 해제 ───────────────────────────────────
        void Register(IRenderable* renderable)
        {
            m_renderables.push_back(renderable);
        }

        void Unregister(IRenderable* renderable)
        {
            m_renderables.erase(
                std::remove(m_renderables.begin(), m_renderables.end(), renderable),
                m_renderables.end());
        }

        // ── Scene::Render()에서 호출 ──────────────────────
        void Render(ID2D1DeviceContext* ctx)
        {
            if (!ctx) return;

            Collect();
            Sort();
            Flush(ctx);
            m_renderItems.clear();
        }

        void Clear()
        {
            m_renderables.clear();
            m_renderItems.clear();
        }

    private:
        std::vector<IRenderable*> m_renderables;  // 등록된 IRenderable
        std::vector<RenderItem>   m_renderItems;  // 매 프레임 재사용

        // IRenderable → RenderItem 수집
        void Collect()
        {
            m_renderItems.reserve(m_renderables.size());
            for (auto* r : m_renderables)
            {
                RenderItem item;
                r->FillRenderItem(item);
                if (item.bitmap)
                    m_renderItems.push_back(item);
            }
        }

        // sortingLayer → orderInLayer 순 정렬
        void Sort()
        {
            std::sort(m_renderItems.begin(), m_renderItems.end(),
                [](const RenderItem& a, const RenderItem& b)
                {
                    if (a.sortingLayer != b.sortingLayer)
                        return a.sortingLayer < b.sortingLayer;
                    return a.orderInLayer < b.orderInLayer;
                });
        }

        // 일괄 드로우콜
        void Flush(ID2D1DeviceContext* ctx)
        {
            for (auto& item : m_renderItems)
            {
                // SimpleMath::Matrix → D2D1_MATRIX_3X2_F 변환
                // SimpleMath Matrix는 행 우선(row-major)
                // D2D는 행렬 구성이 다르므로 _41, _42가 이동(translation)
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

            // 변환 초기화
            ctx->SetTransform(D2D1::Matrix3x2F::Identity());
        }
    };
}