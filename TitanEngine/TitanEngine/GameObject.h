// GameObject.h
#pragma once
#include "Transform.h"
#include "Component.h"
#include <string>
#include <vector>

namespace TitanEngine::SceneManagement { class Scene; }
namespace TitanEngine
{
    class GameObject final : public Object
    {
        friend class SceneManagement::Scene;
        friend class Component;
        friend class Transform;

    public:
        GameObject() = default;
        GameObject(const std::string& name) : m_name(name) {}
        ~GameObject();
        Object* Clone() override;

        Transform* GetTransform() { return m_transform; }
        const std::string& GetName()             const { return m_name; }
        void               SetName(const std::string& name) { m_name = name; }
        bool               GetActive()           const { return m_isActive; }
        void               SetActive(bool value)
        {
            if (m_isActive == value) return;

            m_isActive = value;

            for (Component* comp : m_components)
            {
                if (value)
                    comp->OnEnable();
                else
                    comp->OnDisable();
            }
        }

        template<typename T>
        T* GetComponent()
        {
            for (Component* comp : m_components)
            {
                T* result = dynamic_cast<T*>(comp);
                if (result) return result;
            }
            return nullptr;
        }

        const std::vector<Component*>& GetComponents()
        {
            return m_components;
        }

        template<typename T>
        T* AddComponent()
        {
            static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

            T* comp = new T();
            comp->m_owner = this;
            m_components.push_back(comp);
            RegisterComponent(comp);

            return comp;
        }

    private :
        void RegisterComponent(Component* comp); 

    private:
        Transform* m_transform = nullptr;  // Scene이 소유, 포인터만 참조
        std::string             m_name = "EmptyObject";
        bool                    m_isActive = true;
        std::vector<Component*> m_components;
    };
}