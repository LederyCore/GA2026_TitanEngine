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

        // 각 컴포넌트가 자신의 TypeId 반환
        virtual TypeId GetTypeId() const = 0;

        virtual void Awake() {}
        virtual void OnEnable() {}
        virtual void Start() {}
        virtual void FixedUpdate(float fixedTime) {}
        virtual void Update(float deltaTime) {}
        virtual void LateUpdate(float deltaTime) {}
        virtual void OnDisable() {}
        virtual void OnDestroy() {}

    public:
        bool enabled = true;

    protected:
        GameObject* GetGameObject() const { return m_owner; }

    private:
        friend class GameObject;
        GameObject* m_owner = nullptr;
    };

    // 모든 커스텀 컴포넌트가 이걸 상속
    template<typename T>
    class ComponentBase : public Component
    {
    public:
        // 자동으로 TypeId 구현
        TypeId GetTypeId() const override
        {
            return TitanEngine::GetTypeId<T>();
        }

        // 정적으로도 접근 가능
        static TypeId StaticTypeId()
        {
            return TitanEngine::GetTypeId<T>();
        }
    };
}