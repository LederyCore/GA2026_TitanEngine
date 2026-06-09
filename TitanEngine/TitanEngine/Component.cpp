#include "pch.h"
#include "Component.h"
#include "GameObject.h"
#include "Scene.h"

namespace TitanEngine
{
	void Component::SetActive(bool value)
	{
        if (m_isActive == value) return;
        m_isActive = value;

        auto* scene = m_currentScene;
        if (!scene) return;

        if (value)
        {
            scene->RegisterComponent(this);
            OnEnable();
        }
        else
        {
            scene->UnRegisterComponent(this);
            OnDisable();
        }
	}

	void Component::InitClone(Component* clone)
	{
		clone->m_owner = nullptr;
		clone->m_isActive = m_isActive;
	}
}