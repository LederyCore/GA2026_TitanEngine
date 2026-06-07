#pragma once
#include <cstdint>

namespace TitanEngine
{
    class GameObject;

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
        virtual ~Component() = default;

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
        void SetEnabled(bool value)
        {
            if (m_enabled == value) return;
            m_enabled = value;
            if (m_enabled) OnEnable();
            else           OnDisable();
        }

        bool IsActiveInHierarchy() const;

        GameObject* GetGameObject() const { return m_owner; }

    protected:
        GameObject* m_owner = nullptr;

    private:
        friend class GameObject;
        bool m_enabled = true;
    };

    // ComponentBase - IRenderable 상속 없음
    template<typename T>
    class ComponentBase : public Component
    {
    public:
        TypeId GetTypeId() const override { return TitanEngine::GetTypeId<T>(); }
        static TypeId StaticTypeId() { return TitanEngine::GetTypeId<T>(); }
    };
}