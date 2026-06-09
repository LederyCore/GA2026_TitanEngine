#include "pch.h"
#include "Component.h"
#include "GameObject.h"
#include "SceneManager.h"

namespace TitanEngine
{
	void Component::InitClone(Component* clone)
	{
		clone->m_owner = nullptr;
		clone->m_isActive = m_isActive;
	}
}