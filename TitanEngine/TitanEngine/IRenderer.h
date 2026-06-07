#pragma once

namespace TitanEngine { class RenderSystem; }

namespace TitanEngine::Renderer
{
    class IRenderer
    {
    public:
        virtual ~IRenderer() = default;

        virtual bool Initialize() = 0;
        virtual void ShutDown() = 0;
        virtual void RenderBegin() = 0;
        virtual void RenderEnd() = 0;
        virtual void ShowFPS(float fps) = 0;
        virtual void RenderScene(TitanEngine::RenderSystem* rs) = 0;
        virtual void DrawCircle(float x, float y, float radius,
            const D2D1::ColorF& color) = 0;
    };
}