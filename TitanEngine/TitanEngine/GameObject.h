#pragma once
#include "Transform.h"
#include "Component.h"
#include "Scene.h"
#include <string>
#include <vector>
#include <memory>

namespace TitanEngine::SceneManagement { class Scene; }

namespace TitanEngine
{
    class GameObject final
    {
    public:
        Transform transform;

    public:
        GameObject() = delete;
        explicit GameObject(const std::string& name);
        ~GameObject();

        const std::string& GetName()  const { return m_name; }
        bool               IsActive() const { return m_isActive; }
        void               SetActive(bool value);

        // ── 컴포넌트 ─────────────────────────────────────────
        template<typename T>
        T& AddComponent()
        {
            static_assert(std::is_base_of_v<Component, T>,
                "T must derive from Component");

            auto comp = std::make_unique<T>();
            T* ptr = comp.get();
            ptr->m_owner = this;

            m_components.push_back(std::move(comp));

            // Scene 접근은 cpp 함수로 위임
            if (m_scene)
            {
                RegisterToSystems(ptr);
                NotifyStart(ptr);       // ← Scene 접근 cpp로 분리
            }

            ptr->Awake();
            if (m_isActive && ptr->IsEnabled()) ptr->OnEnable();

            return *ptr;
        }

        template<typename T>
        void RemoveComponent()
        {
            static_assert(std::is_base_of_v<Component, T>,
                "T must derive from Component");

            auto it = std::find_if(m_components.begin(), m_components.end(),
                [](const std::unique_ptr<Component>& c) {
                    return dynamic_cast<T*>(c.get()) != nullptr;
                });

            if (it == m_components.end()) return;

            Component* ptr = it->get();
            if (m_scene) UnregisterFromSystems(ptr);
            ptr->OnDisable();
            ptr->OnDestroy();

            m_components.erase(it);
        }

        template<typename T>
        T* GetComponent() const
        {
            static_assert(std::is_base_of_v<Component, T>,
                "T must derive from Component");

            for (auto& comp : m_components)
                if (T* ptr = dynamic_cast<T*>(comp.get()))
                    return ptr;

            return nullptr;
        }

        template<>
        inline Transform* GetComponent<Transform>() const
        {
            return const_cast<Transform*>(&transform);
        }

        template<typename T>
        std::vector<T*> GetComponents() const
        {
            static_assert(std::is_base_of_v<Component, T>,
                "T must derive from Component");

            std::vector<T*> result;
            for (auto& comp : m_components)
                if (T* ptr = dynamic_cast<T*>(comp.get()))
                    result.push_back(ptr);

            return result;
        }

        // ── 씬그래프 API ─────────────────────────────────────
        void AddChild(GameObject* child);
        void RemoveFromParent();

        GameObject* GetParent() const
        {
            return transform.parent
                ? transform.parent->gameObject()
                : nullptr;
        }

        GameObject* GetFirstChild() const
        {
            return transform.firstChild
                ? transform.firstChild->gameObject()
                : nullptr;
        }

        GameObject* GetNextSibling() const
        {
            return transform.nextSibling
                ? transform.nextSibling->gameObject()
                : nullptr;
        }

        // ── 씬 편입/이탈 ─────────────────────────────────────
        void OnEnterScene(SceneManagement::Scene* scene);
        void OnExitScene();

        SceneManagement::Scene* GetScene() const { return m_scene; }

    private:
        void NotifyStart(Component* comp);  // ← 추가
        void RegisterToSystems(Component* comp);
        void UnregisterFromSystems(Component* comp);
        void OnEnableAllComponents();
        void OnDisableAllComponents();

    private:
        friend class Component;

        std::string m_name;
        bool        m_isActive = true;

        std::vector<std::unique_ptr<Component>> m_components;

        SceneManagement::Scene* m_scene = nullptr;
    };
}