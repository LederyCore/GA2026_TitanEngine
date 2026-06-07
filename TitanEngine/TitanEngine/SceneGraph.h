#pragma once
#include "Transform.h"
#include <vector>
#include <directxtk/SimpleMath.h>

namespace TitanEngine::SceneManagement
{
    // WorldMatrix 전파 전용
    // 게임 로직 루프는 UpdateSystem이 담당함..
    class SceneGraph final
    {
    public:
        SceneGraph() = default;
        ~SceneGraph() = default;

        void AddRoot(Transform* root);
        void RemoveRoot(Transform* root);
        void Clear();

        // WorldMatrix 전파만 수행 (매 프레임 1회)
        void PropagateWorldMatrix();

    private:
        std::vector<Transform*> m_roots;

        void PropagateRecursive(Transform& node,
            const DirectX::SimpleMath::Matrix& parentWorld);
    };
}