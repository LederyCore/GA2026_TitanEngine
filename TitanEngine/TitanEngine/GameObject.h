#pragma once
#include "Transform.h"
#include "Component.h"
#include <string>
#include <vector>
#include <memory>

namespace TitanEngine::SceneManagement { class Scene; }

namespace TitanEngine
{
    class GameObject final : public Object
    {
        friend class SceneManagement::Scene;
        friend class Component;

    protected :
        Object* Clone() override;

    private:
        std::string m_name;
        bool m_isActive = true;

        GameObject* m_parent = nullptr;
        std::vector<GameObject*> m_children;
        std::vector<Component*> m_components;

        SceneManagement::Scene* m_currentScene = nullptr;
    };
}