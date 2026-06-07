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
        // Start 미실행 컴포넌트 처리
        if (!m_pendingStart.empty())
        {
            for (auto* comp : m_pendingStart)
            {
                if (!comp->IsActiveInHierarchy()) continue;
                comp->Start();
            }
            m_pendingStart.clear();
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