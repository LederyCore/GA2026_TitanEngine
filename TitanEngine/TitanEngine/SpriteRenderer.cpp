#include "pch.h"
#include "SpriteRenderer.h"
#include "GameObject.h"

void TitanEngine::SpriteRenderer::FillRenderItem(RenderItem& out) const
{
    out.bitmap = sprite;
    out.size = size;
    out.opacity = opacity;
    out.sortingLayer = sortingLayer;
    out.orderInLayer = orderInLayer;

    // GameObjectÀÇ WorldMatrix »ç¿ë
    if (m_owner)
        out.worldTransform = m_owner->transform.worldMatrix;
}
