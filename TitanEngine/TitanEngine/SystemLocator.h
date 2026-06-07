#pragma once

namespace TitanEngine
{
    class UpdateSystem;
    class RenderSystem;

    // 전역 접근점 - Scene이 시작/종료 시 등록/해제
    class SystemLocator final
    {
    public:
        static void Set(UpdateSystem* us, RenderSystem* rs)
        {
            s_updateSystem = us;
            s_renderSystem = rs;
        }

        static void Clear()
        {
            s_updateSystem = nullptr;
            s_renderSystem = nullptr;
        }

        static UpdateSystem* GetUpdateSystem() { return s_updateSystem; }
        static RenderSystem* GetRenderSystem() { return s_renderSystem; }

    private:
        static UpdateSystem* s_updateSystem;
        static RenderSystem* s_renderSystem;
    };
}