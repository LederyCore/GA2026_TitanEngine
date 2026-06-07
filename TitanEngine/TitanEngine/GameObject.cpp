#include "pch.h"
#include "GameObject.h"
#include "SystemLocator.h"

TitanEngine::GameObject::GameObject(const std::string& name)
    : m_name(name)
{
    transform.m_owner = this;
    transform.Awake();
}

void TitanEngine::GameObject::SetActive(bool value)
{
    if (m_isActive == value) return;
    m_isActive = value;

    for (auto& [id, comps] : m_components)
        for (auto& comp : comps)
        {
            if (!comp->IsEnabled()) continue;
            if (m_isActive) comp->OnEnable();
            else            comp->OnDisable();
        }
}