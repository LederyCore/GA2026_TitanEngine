#include "pch.h"
#include "PhysicsSystem.h"

namespace TitanEngine
{
    void PhysicsSystem::FixedUpdate(float fixedTime)
    {
        for (auto* p : m_physics)
        {
            if (!p->IsPhysicsActive()) continue;
            p->OnPhysicsUpdate(fixedTime);
        }
    }
}