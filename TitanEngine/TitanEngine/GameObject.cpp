#include "pch.h"
#include "GameObject.h"
#include "SceneManager.h"

namespace TitanEngine
{
	Object* GameObject::Clone()
	{
        // 1. 얕은 복사로 시작
        GameObject* clone = new GameObject(*this);

        // 2. 계층 초기화 (원본 참조 끊기)
        clone->m_parent = nullptr;
        clone->m_children.clear();
        clone->m_components.clear();
        clone->m_currentScene = SceneManagement::SceneManager::Instance().GetActiveScene();

        // 3. 컴포넌트 깊은 복사
        for (Component* comp : m_components)
        {
            Component* compClone = static_cast<Component*>(comp->Clone());
            compClone->m_owner = clone;  // 소유자 재연결
            clone->m_components.push_back(compClone);
        }

        // 4. 자식 GameObject 재귀 복사
        for (GameObject* child : m_children)
        {
            GameObject* childClone = static_cast<GameObject*>(child->Clone());
            childClone->m_parent = clone;
            clone->m_children.push_back(childClone);
        }

        return clone;
	}
}