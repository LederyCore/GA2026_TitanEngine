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

        // 등록 및 해제 (AddComponent 시 1회만 호출됨)
        void RegisterUpdate(Component* comp) { m_updatables.push_back(comp); }
        void RegisterFixed(Component* comp) { m_fixedUpdatables.push_back(comp); }
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

        // 게임 루프 - 연속 배열 순회 = 캐시 친화
        void FixedUpdate(float fixedTime)
        {
            for (auto* comp : m_fixedUpdatables)
                if (comp->enabled)
                    comp->FixedUpdate(fixedTime);
        }

        void Update(float deltaTime)
        {
            for (auto* comp : m_updatables)
                if (comp->enabled)
                    comp->Update(deltaTime);
        }

        void LateUpdate(float deltaTime)
        {
            for (auto* comp : m_lateUpdatables)
                if (comp->enabled)
                    comp->LateUpdate(deltaTime);
        }

        void Clear()
        {
            m_updatables.clear();
            m_fixedUpdatables.clear();
            m_lateUpdatables.clear();
        }

    private:
        std::vector<Component*> m_updatables;        // Update 오버라이드한 것만
        std::vector<Component*> m_fixedUpdatables;   // FixedUpdate 오버라이드한 것만
        std::vector<Component*> m_lateUpdatables;    // LateUpdate 오버라이드한 것만
    };
}