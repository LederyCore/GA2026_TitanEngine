#include "pch.h"
#include "Transform.h"
#include "GameObject.h"
#include "Scene.h"

namespace TitanEngine
{
    Transform* Transform::GetParent() const
    {
        if (m_parentIndex == -1) return nullptr;
        auto* scene = static_cast<SceneManagement::Scene*>(m_owner->GetScene());
        return scene ? scene->GetTransform(m_parentIndex) : nullptr;
    }

    Transform* Transform::GetChild(int index) const
    {
        auto* scene = static_cast<SceneManagement::Scene*>(m_owner->GetScene());
        return scene ? scene->GetTransform(m_childrenIndices[index]) : nullptr;
    }

    void Transform::SetParent(Transform* parent)
    {
        auto* scene = m_owner ? static_cast<SceneManagement::Scene*>(m_owner->GetScene()) : nullptr;
        if (!scene) return;

        // 기존 부모에서 제거
        if (m_parentIndex != -1)
        {
            Transform* oldParent = scene->GetTransform(m_parentIndex);
            auto& siblings = oldParent->m_childrenIndices;
            siblings.erase(
                std::remove(siblings.begin(), siblings.end(), m_selfIndex),
                siblings.end()
            );
        }

        if (parent == nullptr)
        {
            m_parentIndex = -1;
            return;
        }

        m_parentIndex = parent->m_selfIndex;
        parent->m_childrenIndices.push_back(m_selfIndex);

        // 부모가 자식보다 뒤에 있으면 swap
        if (m_parentIndex > m_selfIndex)
            scene->SwapTransforms(m_parentIndex, m_selfIndex);
    }

    Object* Transform::Clone()
    {
        Transform* clone = new Transform(*this);
        InitClone(clone);

        clone->m_selfIndex = -1;
        clone->m_parentIndex = -1;
        clone->m_childrenIndices.clear();
        clone->m_owner = nullptr;

        return clone;
    }
}