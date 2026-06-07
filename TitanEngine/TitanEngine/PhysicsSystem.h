#pragma once
#include "IPhysics.h"
#include <vector>
#include <algorithm>

namespace TitanEngine
{
    class PhysicsSystem final
    {
    public:
        PhysicsSystem() = default;
        ~PhysicsSystem() = default;

        void Register(IPhysics* physics) { m_physics.push_back(physics); }
        void Unregister(IPhysics* physics)
        {
            m_physics.erase(
                std::remove(m_physics.begin(), m_physics.end(), physics),
                m_physics.end());
        }

        void FixedUpdate(float fixedTime);

        void Clear() { m_physics.clear(); }

    private:
        std::vector<IPhysics*> m_physics;
    };
}