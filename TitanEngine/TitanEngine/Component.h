#pragma once
#include "Object.h"

namespace TitanEngine
{
    class GameObject;
    class IFixedUpdateable
    {
    public :
        virtual void FixedUpdate(float fixedTime) = 0;
        virtual ~IFixedUpdateable() = default;
    };

    class IUpdateable
    {
    public:
        virtual void Update(float deltaTime) = 0;
        virtual ~IUpdateable() = default;
    };

    class ILateUpdateable
    {
    public:
        virtual void LateUpdate(float deltaTime) = 0;
        virtual ~ILateUpdateable() = default;
    };

    class IRenderable
    {
    public:
        virtual void Render(ID2D1DeviceContext7* ctx) = 0;
        virtual ~IRenderable() = default;
    };

    class Component : protected Object
    {
        friend class GameObject;
        friend class SceneManagement::Scene;
        GameObject* m_owner = nullptr;

    public :
        virtual ~Component() = default;
        Object* Clone() override = 0;

        virtual void OnAwake() {}
        virtual void OnEnable() {}
        virtual void OnStart() {}
        virtual void OnDisable() {}
        virtual void OnDestory() {}

        const bool GetActive() { return m_isActive; }
        void SetActive(bool value);
     
        const GameObject* GetGameGameObject() { return m_owner; }

    protected :
        void InitClone(Component* clone);

    private :
        bool m_isActive = true;

    };
}