// SceneGraph.h
#pragma once
#include "Transform.h"
#include <directxtk/SimpleMath.h>

namespace TitanEngine { class GameObject; }
namespace TitanEngine::SceneManagement { class Scene; }

namespace TitanEngine::SceneManagement
{
    class SceneGraph
    {
    public:
        SceneGraph() = default;
        ~SceneGraph() = default;

        // 루트에 오브젝트 추가
        void AddToRoot(GameObject* go, Scene* scene);

        // 매 프레임 Engine에서 호출
        void PropagateWorldMatrix();

    private:
        void Propagate(Transform* tr,
            const DirectX::SimpleMath::Matrix& parentWorld);

        // 루트 자식들 (더미 루트 대신 첫 번째 루트 자식 포인터)
        Transform* m_rootFirstChild = nullptr;
    };
}