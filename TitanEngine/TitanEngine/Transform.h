#pragma once
#include "Component.h"
#include <directxtk/SimpleMath.h>

namespace TitanEngine
{
    class Transform : public ComponentBase<Transform>
    {
        using Vector2 = DirectX::SimpleMath::Vector2;
        using Matrix = DirectX::SimpleMath::Matrix;

    public :
        // Hierarchy 조작
        void SetParent(Transform* newParent);
        void DetachFromParent();
        void AddChild(Transform* child);

        // SceneGraph에서 호출
        void UpdateWorldMatrix(const Matrix& parentWorld);

    public:
        // 침투형 링크드 리스트 계층구조
        Transform* parent = nullptr;
        Transform* firstChild = nullptr;
        Transform* nextSibling = nullptr;

        // Local space
        Vector2 localPosition = {};
        float   localRotation = 0.f;
        Vector2 localScale = { 1.f, 1.f };

        //Cached world matrix
        Matrix worldMatrix = {};

    private:
        Matrix CalcLocalMatrix() const;
    };
}