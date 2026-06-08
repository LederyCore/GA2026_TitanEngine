#include "pch.h"
#include "UpdateSystem.h"

namespace TitanEngine::SceneManagement
{
    void UpdateSystem::Register(IUpdatable* updatable)
    {
        if (!updatable) return;

        Component* comp = dynamic_cast<Component*>(updatable);
        if (!comp) return;

        m_updatables.push_back({ comp, updatable });
    }

    void UpdateSystem::Unregister(IUpdatable* updatable)
    {
        auto it = std::find_if(m_updatables.begin(), m_updatables.end(),
            [updatable](const Entry& e) { return e.updatable == updatable; });

        if (it != m_updatables.end())
            m_updatables.erase(it);
    }

    void UpdateSystem::RegisterStart(Component* comp)
    {
        if (!comp) return;
        m_startQueue.push_back(comp);
    }

    void UpdateSystem::FlushStartQueue()
    {
        // 순회 중 재진입 대비 — 먼저 꺼내고 비움
        std::vector<Component*> queue = std::move(m_startQueue);

        for (auto* comp : queue)
            if (comp->IsActiveInHierarchy())
                comp->Start();
    }

    void UpdateSystem::FixedUpdate(float fixedTime)
    {
        for (auto& e : m_updatables)
            if (e.comp->IsActiveInHierarchy())
                e.updatable->FixedUpdate(fixedTime);
    }

    void UpdateSystem::Update(float deltaTime)
    {
        FlushStartQueue();  // Update 직전 Start 처리

        for (auto& e : m_updatables)
            if (e.comp->IsActiveInHierarchy())
                e.updatable->Update(deltaTime);
    }

    void UpdateSystem::LateUpdate(float deltaTime)
    {
        for (auto& e : m_updatables)
            if (e.comp->IsActiveInHierarchy())
                e.updatable->LateUpdate(deltaTime);
    }
}