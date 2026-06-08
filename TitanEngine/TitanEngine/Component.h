#pragma once
#include "SystemLocator.h"
#include <cstdint>

namespace TitanEngine
{
    class GameObject;
    class UpdateSystem;

    using TypeId = uintptr_t;

    template<typename T>
    TypeId GetTypeId()
    {
        static char dummy;
        return reinterpret_cast<TypeId>(&dummy);
    }

    class Component
    {
    public:
        Component() = default;

        // 소멸 시 UpdateSystem 자동 해제
        virtual ~Component();

        virtual TypeId GetTypeId() const = 0;

        virtual void Awake() {}
        virtual void OnEnable() {}
        virtual void Start() {}
        virtual void FixedUpdate(float fixedTime) {}
        virtual void Update(float deltaTime) {}
        virtual void LateUpdate(float deltaTime) {}
        virtual void OnDisable() {}
        virtual void OnDestroy() {}

        bool IsEnabled() const { return m_enabled; }
        void SetEnabled(bool value);

        bool IsActiveInHierarchy() const;

        GameObject* GetGameObject() const { return m_owner; }

    protected:
        GameObject* m_owner = nullptr;

    private:
        friend class GameObject;
        bool m_enabled = true;
    };

    template<typename T>
    class ComponentBase : public Component
    {
    public:
        TypeId GetTypeId() const override { return TitanEngine::GetTypeId<T>(); }
        static TypeId StaticTypeId() { return TitanEngine::GetTypeId<T>(); }
    };
}