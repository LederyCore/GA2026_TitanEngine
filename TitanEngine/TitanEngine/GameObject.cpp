#include "pch.h"
#include "GameObject.h"
#include "Scene.h"

namespace TitanEngine
{
    GameObject::GameObject(const std::string& name)
        : m_name(name)
    {
        transform.m_owner = this;
    }

    GameObject::~GameObject()
    {
        for (auto& comp : m_components)
            comp->OnDestroy();
    }

    void GameObject::NotifyStart(Component* comp)
    {
        // 여기서는 Scene.h가 완전 정의로 보임
        m_scene->GetUpdateSystem()->RegisterStart(comp);
    }

    // ── 시스템 등록/해제 ─────────────────────────────────────
    void GameObject::RegisterToSystems(Component* comp)
    {
        if (!m_scene) return;

        if (auto* u = dynamic_cast<IUpdatable*>(comp))
            m_scene->GetUpdateSystem()->Register(u);

        if (auto* r = dynamic_cast<IRenderable*>(comp))
            m_scene->GetRenderSystem()->Register(r);
    }

    void GameObject::UnregisterFromSystems(Component* comp)
    {
        if (!m_scene) return;

        if (auto* u = dynamic_cast<IUpdatable*>(comp))
            m_scene->GetUpdateSystem()->Unregister(u);

        if (auto* r = dynamic_cast<IRenderable*>(comp))
            m_scene->GetRenderSystem()->Unregister(r);
    }

    // ── 씬 편입/이탈 ─────────────────────────────────────────
    void GameObject::OnEnterScene(SceneManagement::Scene* scene)
    {
        m_scene = scene;

        for (auto& comp : m_components)
            RegisterToSystems(comp.get());

        for (Transform* child = transform.firstChild;
            child;
            child = child->nextSibling)
        {
            child->gameObject()->OnEnterScene(scene);
        }
    }

    void GameObject::OnExitScene()
    {
        for (auto& comp : m_components)
            UnregisterFromSystems(comp.get());

        for (Transform* child = transform.firstChild;
            child;
            child = child->nextSibling)
        {
            child->gameObject()->OnExitScene();
        }

        m_scene = nullptr;
    }

    // ── SetActive ─────────────────────────────────────────────
    void GameObject::SetActive(bool value)
    {
        if (m_isActive == value) return;
        m_isActive = value;

        if (m_isActive) OnEnableAllComponents();
        else            OnDisableAllComponents();

        for (Transform* child = transform.firstChild;
            child;
            child = child->nextSibling)
        {
            child->gameObject()->SetActive(value);
        }
    }

    void GameObject::OnEnableAllComponents()
    {
        for (auto& comp : m_components)
            if (comp->IsEnabled()) comp->OnEnable();
    }

    void GameObject::OnDisableAllComponents()
    {
        for (auto& comp : m_components)
            if (comp->IsEnabled()) comp->OnDisable();
    }

    // ── 씬그래프 ─────────────────────────────────────────────
    void GameObject::AddChild(GameObject* child)
    {
        if (!child || child == this) return;

        child->transform.parent = &this->transform;
        child->transform.nextSibling = this->transform.firstChild;
        child->transform.prevSibling = nullptr;

        if (this->transform.firstChild)
            this->transform.firstChild->prevSibling = &child->transform;

        this->transform.firstChild = &child->transform;

        if (m_scene) child->OnEnterScene(m_scene);
    }

    void GameObject::RemoveFromParent()
    {
        if (!transform.parent) return;

        if (transform.prevSibling)
            transform.prevSibling->nextSibling = transform.nextSibling;
        else
            transform.parent->firstChild = transform.nextSibling;

        if (transform.nextSibling)
            transform.nextSibling->prevSibling = transform.prevSibling;

        transform.parent = nullptr;
        transform.nextSibling = nullptr;
        transform.prevSibling = nullptr;

        if (m_scene) OnExitScene();
    }
}