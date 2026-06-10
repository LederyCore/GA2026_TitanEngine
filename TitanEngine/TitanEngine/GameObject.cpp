#include "pch.h"
#include "GameObject.h"
#include "Scene.h"

namespace TitanEngine
{
    GameObject::~GameObject()
    {
        for (Component* comp : m_components)
        {
            // 컴포넌트의 다형성이 유지되는 delete 전에 명시적으로 호출합니다.
            if (comp->GetActive())
            {
                comp->OnDisable();
            }
            comp->OnDestory();

            delete comp;
        }
        m_components.clear();
    }

    Object* GameObject::Clone()
    {
        GameObject* clone = new GameObject(m_name);

        clone->m_isActive = m_isActive;

        for (Component* comp : m_components)
        {
            Component* compClone = static_cast<Component*>(comp->Clone());
            compClone->m_owner = clone;
            clone->m_components.push_back(compClone);
        }

        if (m_transform)
        {
            for (int i = 0; i < m_transform->GetChildCount(); i++)
            {
                Transform* childTransform = m_transform->GetChild(i);
                GameObject* childClone = static_cast<GameObject*>(
                    childTransform->GetOwner()->Clone()
                    );
                if (childClone->m_transform)
                    childClone->m_transform->SetParent(clone->m_transform);
            }
        }

        return clone;
    }
    void GameObject::RegisterComponent(Component* comp)
    {
        if (!m_currentScene) return;
        m_currentScene->RegisterComponent(comp);  // Scene에 위임

        comp->OnAwake();          // 즉시 호출

        if (comp->GetActive())
        {
            comp->OnEnable();     // 즉시 호출

            // OnStart는 다음 프레임 시작 시 호출되도록 예약
            if (m_currentScene)
                m_currentScene->AddToPendingStartList(comp);
        }
    }
}