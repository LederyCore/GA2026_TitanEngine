#include "pch.h"
#include "Component.h"
#include "GameObject.h"

namespace TitanEngine
{
    bool Component::IsActiveInHierarchy() const
    {
        return m_enabled && m_owner && m_owner->IsActive();
    }

    void Component::SetEnabled(bool value)
    {
        if (m_enabled == value) return;
        m_enabled = value;

        // 오브젝트 자체가 꺼져 있으면 OnEnable/OnDisable 전파 안 함
        if (!m_owner || !m_owner->IsActive()) return;

        if (m_enabled) OnEnable();
        else           OnDisable();
    }
}