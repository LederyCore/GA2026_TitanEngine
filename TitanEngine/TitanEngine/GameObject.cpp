#include "pch.h"
#include "GameObject.h"

TitanEngine::GameObject::GameObject(const std::string& name)
{
	m_name = name;
	transform = AddComponent<Transform>();
}
