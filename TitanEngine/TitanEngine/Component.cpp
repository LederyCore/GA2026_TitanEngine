#include "pch.h"
#include "Component.h"
#include "GameObject.h"

namespace TitanEngine
{
    bool Component::IsActiveInHierarchy() const
    {
        return m_enabled && m_owner && m_owner->IsActive();
    }
}