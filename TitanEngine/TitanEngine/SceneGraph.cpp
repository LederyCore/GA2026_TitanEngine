#include "pch.h"
#include "SceneGraph.h"
#include <algorithm>

namespace TitanEngine::SceneManagement
{
    void SceneGraph::AddRoot(Transform* root)
    {
        m_roots.push_back(root);
    }

    void SceneGraph::RemoveRoot(Transform* root)
    {
        m_roots.erase(std::remove(m_roots.begin(), m_roots.end(), root),
            m_roots.end());
    }

    void SceneGraph::Clear()
    {
        m_roots.clear();
    }

    void SceneGraph::PropagateWorldMatrix()
    {
        for (Transform* root : m_roots)
            PropagateRecursive(*root, DirectX::SimpleMath::Matrix{});
    }

    void SceneGraph::PropagateRecursive(Transform& node,
        const DirectX::SimpleMath::Matrix& parentWorld)
    {
        // 부모 → 자식 순서 보장 (DFS)
        node.UpdateWorldMatrix(parentWorld);

        Transform* child = node.firstChild;
        while (child)
        {
            PropagateRecursive(*child, node.worldMatrix);
            child = child->nextSibling;
        }
    }
}