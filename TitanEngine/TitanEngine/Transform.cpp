#include "pch.h"
#include "Transform.h"

namespace TitanEngine
{
    void Transform::SetParent(Transform* newParent)
    {
        if (parent == newParent) return;

        DetachFromParent();

        parent = newParent;

        if (newParent)
        {
            // 형제 리스트 앞에 삽입
            nextSibling = newParent->firstChild;
            newParent->firstChild = this;
        }
    }

    void Transform::DetachFromParent()
    {
        if (!parent) return;

        // 형제 리스트에서 자신 제거
        Transform** curr = &parent->firstChild;
        while (*curr)
        {
            if (*curr == this)
            {
                *curr = nextSibling;
                break;
            }
            curr = &(*curr)->nextSibling;
        }

        parent = nullptr;
        nextSibling = nullptr;
    }

    void Transform::AddChild(Transform* child)
    {
        if (child)
            child->SetParent(this);
    }

    void Transform::UpdateWorldMatrix(const DirectX::SimpleMath::Matrix& parentWorld)
    {
        worldMatrix = CalcLocalMatrix() * parentWorld;
    }

    DirectX::SimpleMath::Matrix Transform::CalcLocalMatrix() const
    {
        using namespace DirectX::SimpleMath;

        Matrix scale = Matrix::CreateScale(localScale.x, localScale.y, 1.f);
        Matrix rotation = Matrix::CreateRotationZ(DirectX::XMConvertToRadians(localRotation));
        Matrix trans = Matrix::CreateTranslation(localPosition.x, localPosition.y, 0.f);

        return scale * rotation * trans;
    }
}