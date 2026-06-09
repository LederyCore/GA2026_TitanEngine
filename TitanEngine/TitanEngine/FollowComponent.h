#pragma once
#include "Component.h"
#include "GameObject.h"
#include <deque>

namespace TitanEngine
{
    class FollowComponent : public Component, public IUpdateable
    {
    public:
        Object* Clone() override
        {
            FollowComponent* clone = new FollowComponent(*this);
            InitClone(clone);
            return clone;
        }

        void SetTarget(Transform* target) { m_target = target; }
        void SetDelay(float delay) { m_delay = delay; }

        void Update(float deltaTime) override
        {
            if (!m_target) return;

            Transform* t = GetOwner()->GetTransform();
            if (!t) return;

            // 타겟 월드 위치 기록
            m_elapsed += deltaTime;
            m_history.push_back({ m_elapsed, m_target->GetWorldPosition() });

            // delay 이전 기록 제거
            while (m_history.size() > 1 &&
                m_elapsed - m_history.front().time > m_delay)
            {
                m_history.pop_front();
            }

            // delay 이전 위치로 이동
            if (!m_history.empty())
            {
                Vector2 targetPos = m_history.front().pos;

                // 자식은 로컬 좌표계 → 부모 월드 역행렬로 변환
                Transform* parent = t->GetParent();
                if (parent)
                {
                    Matrix inv = parent->GetInverseWorldMatrix();
                    Vector3 local = Vector3::Transform(
                        Vector3(targetPos.x, targetPos.y, 0.0f), inv);
                    t->SetLocalPosition(local.x, local.y);
                }
                else
                {
                    t->SetLocalPosition(targetPos.x, targetPos.y);
                }
            }
        }

    private:
        struct PositionRecord
        {
            float   time;
            Vector2 pos;
        };

        Transform* m_target = nullptr;
        float                       m_delay = 0.5f;
        float                       m_elapsed = 0.0f;
        std::deque<PositionRecord>  m_history;
    };
}