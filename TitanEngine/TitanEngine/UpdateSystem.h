#pragma once
#include "Component.h"
#include <vector>
#include <algorithm>

namespace TitanEngine
{
    class UpdateSystem final
    {
    public:
        UpdateSystem() = default;
        ~UpdateSystem() = default;

        void RegisterFixed(Component* comp) { m_fixedUpdatables.push_back(comp); }
        void RegisterStart(Component* comp);
        void RegisterUpdate(Component* comp) { m_updatables.push_back(comp); }
        void RegisterLate(Component* comp) { m_lateUpdatables.push_back(comp); }

        void Unregister(Component* comp)
        {
            auto remove = [&](std::vector<Component*>& vec)
                {
                    vec.erase(std::remove(vec.begin(), vec.end(), comp), vec.end());
                };
            remove(m_updatables);
            remove(m_fixedUpdatables);
            remove(m_lateUpdatables);
        }

        void FixedUpdate(float fixedTime);
        void Update(float deltaTime);
        void LateUpdate(float deltaTime);

        void Clear()
        {
            m_updatables.clear();
            m_fixedUpdatables.clear();
            m_lateUpdatables.clear();
        }

    private:
        std::vector<Component*> m_fixedUpdatables;
        std::vector<Component*> m_pendingStart;
        std::vector<Component*> m_updatables;
        std::vector<Component*> m_lateUpdatables;
    };
}