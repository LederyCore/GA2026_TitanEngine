#pragma once
#include "Transform.h"
#include "SystemLocator.h"
#include "UpdateSystem.h"   
#include "RenderSystem.h"   
#include "IRenderable.h"    
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

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

        const std::string& GetName()             const { return m_name; }
        bool               IsActive()            const { return m_isActive; }
        void               SetActive(bool value);

        template<typename T>
        T& AddComponent()
        {
            static_assert(std::is_base_of<Component, T>::value,
                "T must derive from Component");

            TypeId id = TitanEngine::GetTypeId<T>();
            auto   comp = std::make_unique<T>();
            T* ptr = comp.get();
            ptr->m_owner = this;

            auto* us = SystemLocator::GetUpdateSystem();
            auto* rs = SystemLocator::GetRenderSystem();

            if (us)
            {
                if constexpr (&T::FixedUpdate != &Component::FixedUpdate)
                    us->RegisterFixed(ptr);
                if constexpr (&T::Update != &Component::Update)
                    us->RegisterUpdate(ptr);
                if constexpr (&T::LateUpdate != &Component::LateUpdate)
                    us->RegisterLate(ptr);
            }

            if (rs)
            {
                if (auto* r = dynamic_cast<IRenderable*>(ptr))
                    rs->Register(r);
            }

            m_components[id].push_back(std::move(comp));
            ptr->Awake();
            return *ptr;
        }

        template<typename T>
        T* GetComponent() const
        {
            static_assert(std::is_base_of<Component, T>::value,
                "T must derive from Component");

            TypeId id = TitanEngine::GetTypeId<T>();
            auto   it = m_components.find(id);

            if (it == m_components.end() || it->second.empty())
                return nullptr;

            return static_cast<T*>(it->second[0].get());
        }

        template<>
        inline Transform* GetComponent<Transform>() const
        {
            return const_cast<Transform*>(&transform);
        }

        template<typename T>
        std::vector<T*> GetComponents() const
        {
            static_assert(std::is_base_of<Component, T>::value,
                "T must derive from Component");

            TypeId          id = TitanEngine::GetTypeId<T>();
            std::vector<T*> result;
            auto            it = m_components.find(id);

            if (it == m_components.end()) return result;

            for (auto& comp : it->second)
                result.push_back(static_cast<T*>(comp.get()));

            return result;
        }

    private:
        friend class Component;

        std::string m_name;
        bool        m_isActive = true;

        std::unordered_map<TypeId,
            std::vector<std::unique_ptr<Component>>> m_components;
    };
}