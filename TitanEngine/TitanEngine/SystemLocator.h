#pragma once

namespace TitanEngine
{
    class UpdateSystem;
    class RenderSystem;
    class PhysicsSystem;

    class SystemLocator final
    {
    public:
        static void Set(UpdateSystem* us, RenderSystem* rs, PhysicsSystem* ps)
        {
            s_updateSystem = us;
            s_renderSystem = rs;
            s_physicsSystem = ps;
        }

        static void Clear()
        {
            s_updateSystem = nullptr;
            s_renderSystem = nullptr;
            s_physicsSystem = nullptr;
        }

        static UpdateSystem* GetUpdateSystem() { return s_updateSystem; }
        static RenderSystem* GetRenderSystem() { return s_renderSystem; }
        static PhysicsSystem* GetPhysicsSystem() { return s_physicsSystem; }

    private:
        static UpdateSystem* s_updateSystem;
        static RenderSystem* s_renderSystem;
        static PhysicsSystem* s_physicsSystem;
    };
}