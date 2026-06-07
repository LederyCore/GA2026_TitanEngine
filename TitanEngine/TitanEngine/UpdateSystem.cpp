#include "pch.h"
#include "UpdateSystem.h"
#include "GameObject.h"

namespace TitanEngine
{
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