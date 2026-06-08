#include "pch.h"
#include "Transform.h"

namespace TitanEngine
{
    using Matrix = DirectX::SimpleMath::Matrix;
    using Vector3 = DirectX::SimpleMath::Vector3;
    using Quaternion = DirectX::SimpleMath::Quaternion;

    // ── 로컬 행렬 계산 ────────────────────────────────────────
    Matrix Transform::CalcLocalMatrix() const
    {
        return Matrix::CreateScale(localScale)
            * Matrix::CreateFromQuaternion(localRotation)
            * Matrix::CreateTranslation(localPosition);
    }

    // ── 월드 행렬 갱신 (SceneGraph 호출) ─────────────────────
    void Transform::UpdateWorldMatrix(const Matrix& parentWorld)
    {
        worldMatrix = CalcLocalMatrix() * parentWorld;
    }

    // ── 자식 추가 ─────────────────────────────────────────────
    void Transform::AddChild(Transform* child)
    {
        if (!child || child == this) return;

        // 기존 부모에서 먼저 분리
        child->DetachFromParent();

        child->parent = this;
        child->nextSibling = firstChild;
        child->prevSibling = nullptr;

        if (firstChild)
            firstChild->prevSibling = child;

        firstChild = child;
    }

    // ── 부모 변경 ─────────────────────────────────────────────
    void Transform::SetParent(Transform* newParent)
    {
        if (parent == newParent) return;

        if (newParent) newParent->AddChild(this);
        else           DetachFromParent();
    }

    // ── 부모에서 분리 ─────────────────────────────────────────
    void Transform::DetachFromParent()
    {
        if (!parent) return;

        if (prevSibling)
            prevSibling->nextSibling = nextSibling;
        else
            parent->firstChild = nextSibling;   // 첫째 자식이었던 경우

        if (nextSibling)
            nextSibling->prevSibling = prevSibling;

        parent = nullptr;
        prevSibling = nullptr;
        nextSibling = nullptr;
    }
}