#include "pch.h"
#include "UpdateSystem.h"
#include "GameObject.h"

namespace TitanEngine
{
    void UpdateSystem::RegisterStart(Component* comp)
    {
        m_pendingStart.push_back(comp);
    }
    void UpdateSystem::FixedUpdate(float fixedTime)
    {
        for (auto* comp : m_fixedUpdatables)
        {
            if (!comp->IsEnabled())                 continue;
            if (!comp->GetGameObject()->IsActive()) continue;
            comp->FixedUpdate(fixedTime);
        }
    }

    void UpdateSystem::Update(float deltaTime)
    {
        if (!m_pendingStart.empty())
        {
            std::vector<Component*> stillPending;

            for (auto* comp : m_pendingStart)
            {
                if (!comp->IsActiveInHierarchy())
                {
                    stillPending.push_back(comp); // 스킵된 건 다음 프레임으로 보존
                    continue;
                }
                comp->Start();
            }
            m_pendingStart = std::move(stillPending);
        }

        // 일반 Update
        for (auto* comp : m_updatables)
        {
            if (!comp->IsEnabled())                 continue;
            if (!comp->GetGameObject()->IsActive()) continue;
            comp->Update(deltaTime);
        }
    }

    void UpdateSystem::LateUpdate(float deltaTime)
    {
        for (auto* comp : m_lateUpdatables)
        {
            if (!comp->IsEnabled())                 continue;
            if (!comp->GetGameObject()->IsActive()) continue;
            comp->LateUpdate(deltaTime);
        }
    }
}