#include "pch.h"
#include "RenderSystem.h"

namespace TitanEngine::SceneManagement
{
    void RenderSystem::Register(IRenderable* renderable)
    {
        if (!renderable) return;

        // IRenderable을 구현한 클래스는 반드시 Component도 상속
        Component* comp = dynamic_cast<Component*>(renderable);
        if (!comp) return;

        m_renderables.push_back({ comp, renderable });
    }

    void RenderSystem::Unregister(IRenderable* renderable)
    {
        auto it = std::find_if(m_renderables.begin(), m_renderables.end(),
            [renderable](const Entry& e) { return e.renderable == renderable; });

        if (it != m_renderables.end())
            m_renderables.erase(it);
    }

    void RenderSystem::Draw(ID2D1DeviceContext7* ctx)
    {
        if (!ctx) return;

        for (auto& e : m_renderables)
        {
            if (e.comp->IsActiveInHierarchy())
                e.renderable->Render(ctx);
        }
    }
}