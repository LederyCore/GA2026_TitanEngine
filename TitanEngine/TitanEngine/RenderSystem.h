// RenderSystem.h
#pragma once
#include <vector>
#include <d2d1_3.h>
#include "Component.h"

namespace TitanEngine::SceneManagement
{
    class RenderSystem
    {
    public:
        void Register(IRenderable* renderable);
        void Unregister(IRenderable* renderable);
        void Draw(ID2D1DeviceContext7* ctx);

    private:
        struct Entry 
        {
            Component* comp;       // IsActiveInHierarchy() 접근용
            IRenderable* renderable; // Render() 호출용
        };
        std::vector<Entry> m_renderables;
    };
}