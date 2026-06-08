#pragma once

namespace TitanEngine
{
    class GameObject;

    class Component
    {
    public:
        Component() = default;
        virtual ~Component() = default;

        virtual void Awake() {}
        virtual void OnEnable() {}
        virtual void Start() {}
        virtual void OnDisable() {}
        virtual void OnDestroy() {}

        bool IsEnabled() const { return m_enabled; }
        void SetEnabled(bool value);

        bool IsActiveInHierarchy() const;

        GameObject* gameObject() const { return m_owner; }

    protected:
        GameObject* m_owner = nullptr;

    private:
        friend class GameObject;
        bool m_enabled = true;
    };

    class IUpdatable 
    {
    public:
        virtual ~IUpdatable() = default;
        virtual void FixedUpdate(float fixedTime) {}
        virtual void Update(float deltaTime) {}
        virtual void LateUpdate(float deltaTime) {}
    };

    class IRenderable 
    {
    public:
        virtual ~IRenderable() = default;
        virtual void Render(ID2D1DeviceContext7* ctx) = 0;
    };
}