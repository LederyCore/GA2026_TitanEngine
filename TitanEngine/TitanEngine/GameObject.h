#pragma once
#include "Transform.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace TitanEngine
{
    class GameObject final
    {
    public:
        Transform transform; // 반드시 첫 번째 멤버

    public:
        GameObject() = delete;
        explicit GameObject(const std::string& name);
        ~GameObject() = default;

        template<typename T>
        T& AddComponent()
        {
            TypeId id = TitanEngine::GetTypeId<T>();

            auto  comp = std::make_unique<T>();
            T* ptr = comp.get();
            ptr->m_owner = this;

            m_components[id].push_back(std::move(comp));

            ptr->Awake();
            return *ptr;
        }

        // 단일 반환
        template<typename T>
        T* GetComponent() const
        {
            TypeId id = TitanEngine::GetTypeId<T>();

            auto it = m_components.find(id);
            if (it == m_components.end() || it->second.empty())
                return nullptr;

            // TypeId로 이미 타입이 존재함을 보장
            return static_cast<T*>(it->second[0].get());
        }

        // 동일 타입 전체 반환
        template<typename T>
        std::vector<T*> GetComponents() const
        {
            TypeId id = TitanEngine::GetTypeId<T>();

            std::vector<T*> result;
            auto it = m_components.find(id);
            if (it == m_components.end()) return result;

            for (auto& comp : it->second)
                result.push_back(static_cast<T*>(comp.get()));

            return result;
        }

    private:
        std::string m_name;
        bool        m_isActive = true;

        // TypeId 키의 컴포넌트 배열 (같은 타입 여러 개 허용)
        std::unordered_map<TypeId,
            std::vector<std::unique_ptr<Component>>> m_components;
    };
}