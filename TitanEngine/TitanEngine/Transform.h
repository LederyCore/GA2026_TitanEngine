#pragma once
#include "Component.h"
#include <vector>
#include <d2d1helper.h>

namespace TitanEngine
{
    class Transform : public Component
    {
        friend class SceneManagement::Scene;
        friend class GameObject;

    public:
        Object* Clone() override;

        // 로컬 설정
        void SetLocalPosition(float x, float y) { m_localPosition = { x, y }; }
        void SetLocalRotation(float angle) { m_localRotation = angle; }
        void SetLocalScale(float x, float y) { m_localScale = { x, y }; }

        // 로컬 읽기
        D2D1_POINT_2F GetLocalPosition() const { return m_localPosition; }
        float         GetLocalRotation() const { return m_localRotation; }
        D2D1_POINT_2F GetLocalScale()    const { return m_localScale; }

        // 월드 읽기
        D2D1_POINT_2F           GetWorldPosition() const { return m_worldPosition; }
        float                   GetWorldRotation() const { return m_worldRotation; }
        D2D1_POINT_2F           GetWorldScale()    const { return m_worldScale; }
        const D2D1::Matrix3x2F& GetWorldMatrix()   const { return m_worldMatrix; }
        D2D1::Matrix3x2F GetInverseWorldMatrix() const
        {
            D2D1::Matrix3x2F inv = m_worldMatrix;
            inv.Invert();
            return inv;
        }

        // 계층구조
        void       SetParent(Transform* parent);
        Transform* GetParent()         const;
        Transform* GetChild(int index) const;
        int        GetChildCount()     const { return (int)m_childrenIndices.size(); }
        int        GetSelfIndex()      const { return m_selfIndex; }
        bool       IsRoot()            const { return m_parentIndex == -1; }

        // 소유 GameObject
        GameObject* GetOwner() const { return m_owner; }

    private:
        void SetWorldMatrix(const D2D1::Matrix3x2F& worldMatrix)
        {
            m_worldMatrix = worldMatrix;
            m_worldPosition = { worldMatrix._31, worldMatrix._32 };
            m_worldRotation = atan2f(worldMatrix._12, worldMatrix._11) * (180.0f / 3.14159f);
            m_worldScale = {
                sqrtf(worldMatrix._11 * worldMatrix._11 + worldMatrix._12 * worldMatrix._12),
                sqrtf(worldMatrix._21 * worldMatrix._21 + worldMatrix._22 * worldMatrix._22)
            };
        }

    private:
        // 로컬
        D2D1_POINT_2F    m_localPosition = { 0.0f, 0.0f };
        float            m_localRotation = 0.0f;
        D2D1_POINT_2F    m_localScale = { 1.0f, 1.0f };

        // 월드
        D2D1_POINT_2F    m_worldPosition = { 0.0f, 0.0f };
        float            m_worldRotation = 0.0f;
        D2D1_POINT_2F    m_worldScale = { 1.0f, 1.0f };
        D2D1::Matrix3x2F m_worldMatrix = D2D1::Matrix3x2F::Identity();

        // 인덱스 기반 계층구조
        int              m_selfIndex = -1;
        int              m_parentIndex = -1;
        std::vector<int> m_childrenIndices;

        // 소유 GameObject
        GameObject* m_owner = nullptr;
    };
}