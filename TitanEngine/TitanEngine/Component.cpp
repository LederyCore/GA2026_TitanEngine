#include "pch.h"
#include "Component.h"
#include "GameObject.h"
#include "UpdateSystem.h"

namespace TitanEngine
{
    Component::~Component()
    {
        auto* us = SystemLocator::GetUpdateSystem();
        if (us) us->Unregister(this);
    }
    bool Component::IsActiveInHierarchy() const
    {
        return m_enabled && m_owner && m_owner->IsActive();
    }
}