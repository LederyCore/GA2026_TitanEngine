#pragma once
#include <vector>
#include "Component.h"

namespace TitanEngine::SceneManagement
{
    class UpdateSystem
    {
    public:
        UpdateSystem() = default;
        ~UpdateSystem() = default;

        void Register(IUpdatable* updatable);
        void Unregister(IUpdatable* updatable);
        void RegisterStart(Component* comp);

        void FixedUpdate(float fixedTime);
        void Update(float deltaTime);
        void LateUpdate(float deltaTime);

    private:
        void FlushStartQueue();

        struct Entry 
        {
            Component* comp;
            IUpdatable* updatable;
        };

        std::vector<Entry>      m_updatables;
        std::vector<Component*> m_startQueue;
    };
}