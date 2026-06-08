#pragma once
#include "Component.h"
#include <directxtk/SimpleMath.h>

namespace TitanEngine
{
    class Transform : public Component
    {
        using Vector3 = DirectX::SimpleMath::Vector3;
        using Quaternion = DirectX::SimpleMath::Quaternion;
        using Matrix = DirectX::SimpleMath::Matrix;

    public:
        // 계층 조작
        void SetParent(Transform* newParent);
        void DetachFromParent();
        void AddChild(Transform* child);

        // SceneGraph에서 호출
        void UpdateWorldMatrix(const Matrix& parentWorld);

    public:
        // 침투형 링크드 리스트
        Transform* parent = nullptr;
        Transform* firstChild = nullptr;
        Transform* nextSibling = nullptr;
        Transform* prevSibling = nullptr;   // ← 추가

        // 로컬
        Vector3    localPosition = {};
        Quaternion localRotation = {};
        Vector3    localScale = { 1.f, 1.f, 1.f };

        // 캐시된 월드 행렬
        Matrix worldMatrix = {};

    private:
        Matrix CalcLocalMatrix() const;
    };
}