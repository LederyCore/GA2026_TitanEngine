#include "pch.h"
#include "SpriteRenderer.h"
#include "GameObject.h"

namespace TitanEngine
{
    bool SpriteRenderer::IsRenderActive() const
    {
        return IsActiveInHierarchy();
    }

    void SpriteRenderer::FillRenderItem(RenderItem& out) const
    {
        out.bitmap = sprite;
        out.size = size;
        out.opacity = opacity;
        out.sortingLayer = sortingLayer;
        out.orderInLayer = orderInLayer;

        if (m_owner)
            out.worldTransform = m_owner->transform.worldMatrix;
    }
}