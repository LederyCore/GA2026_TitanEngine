// Transform.h
#pragma once
#include "Component.h"
#include <vector>
#include <DirectXTK/SimpleMath.h>

namespace TitanEngine
{
    using namespace DirectX::SimpleMath;

    class Transform : public Component
    {
        friend class SceneManagement::Scene;
        friend class GameObject;

    public:
        Object* Clone() override;

        // 로컬 설정
        void SetLocalPosition(float x, float y) { m_localPosition = { x, y }; }
        void SetLocalPosition(Vector2 pos) { m_localPosition = pos; }
        void SetLocalRotation(float angle) { m_localRotation = angle; }
        void SetLocalScale(float x, float y) { m_localScale = { x, y }; }
        void SetLocalScale(Vector2 scale) { m_localScale = scale; }

        // 로컬 읽기
        Vector2 GetLocalPosition() const { return m_localPosition; }
        float   GetLocalRotation() const { return m_localRotation; }
        Vector2 GetLocalScale()    const { return m_localScale; }

        // 월드 읽기
        Vector2        GetWorldPosition() const { return m_worldPosition; }
        float          GetWorldRotation() const { return m_worldRotation; }
        Vector2        GetWorldScale()    const { return m_worldScale; }
        const Matrix& GetWorldMatrix()   const { return m_worldMatrix; }

        Matrix GetInverseWorldMatrix() const
        {
            return m_worldMatrix.Invert();  // SimpleMath Matrix는 Invert()가 새 행렬 반환
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
        void SetWorldMatrix(const Matrix& worldMatrix)
        {
            m_worldMatrix = worldMatrix;
            m_worldPosition = { worldMatrix._41, worldMatrix._42 };
            m_worldRotation = atan2f(worldMatrix._12, worldMatrix._11) * (180.0f / 3.14159f);
            m_worldScale = {
                sqrtf(worldMatrix._11 * worldMatrix._11 + worldMatrix._12 * worldMatrix._12),
                sqrtf(worldMatrix._21 * worldMatrix._21 + worldMatrix._22 * worldMatrix._22)
            };
        }

    private:
        // 로컬
        Vector2 m_localPosition = { 0.0f, 0.0f };
        float   m_localRotation = 0.0f;
        Vector2 m_localScale = { 1.0f, 1.0f };

        // 월드
        Vector2 m_worldPosition = { 0.0f, 0.0f };
        float   m_worldRotation = 0.0f;
        Vector2 m_worldScale = { 1.0f, 1.0f };
        Matrix  m_worldMatrix = Matrix{};

        // 인덱스 기반 계층구조
        int              m_selfIndex = -1;
        int              m_parentIndex = -1;
        std::vector<int> m_childrenIndices;

        // 소유 GameObject
        GameObject* m_owner = nullptr;
    };
}