// SceneGraph.cpp
#include "pch.h"
#include "SceneGraph.h"
#include "GameObject.h"
#include "Scene.h"

namespace TitanEngine::SceneManagement
{
    void SceneGraph::AddToRoot(GameObject* go, Scene* scene)
    {
        if (!go) return;

        // 루트 자식 리스트 앞에 삽입
        go->transform.nextSibling = m_rootFirstChild;
        go->transform.prevSibling = nullptr;
        go->transform.parent = nullptr;

        if (m_rootFirstChild)
            m_rootFirstChild->prevSibling = &go->transform;

        m_rootFirstChild = &go->transform;

        // 시스템 등록
        go->OnEnterScene(scene);
    }

    void SceneGraph::PropagateWorldMatrix()
    {
        const auto identity = DirectX::SimpleMath::Matrix{};

        for (auto* tr = m_rootFirstChild; tr; tr = tr->nextSibling)
            Propagate(tr, identity);
    }

    void SceneGraph::Propagate(Transform* tr,
        const DirectX::SimpleMath::Matrix& parentWorld)
    {
        tr->UpdateWorldMatrix(parentWorld);

        for (auto* child = tr->firstChild; child; child = child->nextSibling)
            Propagate(child, tr->worldMatrix);
    }
}