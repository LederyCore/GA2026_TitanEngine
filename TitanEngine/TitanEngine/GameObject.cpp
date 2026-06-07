#include "pch.h"
#include "GameObject.h"
#include "SystemLocator.h"

TitanEngine::GameObject::GameObject(const std::string& name)
    : m_name(name)
{
    transform.m_owner = this;
    transform.Awake();
}

TitanEngine::GameObject::~GameObject()
{
    // SystemLocator에서 모든 컴포넌트 해제
    auto* us = SystemLocator::GetUpdateSystem();
    auto* rs = SystemLocator::GetRenderSystem();

    for (auto& [id, comps] : m_components)
    {
        for (auto& comp : comps)
        {
            if (us) us->Unregister(comp.get());
            if (rs)
            {
                if (auto* r = dynamic_cast<IRenderable*>(comp.get()))
                    rs->Unregister(r);
            }
        }
    }
    // m_components → unique_ptr 자동 해제
}

void TitanEngine::GameObject::SetActive(bool value)
{
    if (m_isActive == value) return;
    m_isActive = value;

    // 모든 컴포넌트에 통보
    for (auto& [id, comps] : m_components)
    {
        for (auto& comp : comps)
        {
            if (!comp->IsEnabled()) continue; // 비활성 컴포넌트는 스킵

            if (m_isActive) comp->OnEnable();
            else            comp->OnDisable();
        }
    }
}