#include "pch.h"
#include "Bubble.h"

#include <DebugConsole/DebugConsole.h>
#include <InputSystem/InputSystem.h>

#include "Animator.h"
#include "GameObject.h"
#include "HitEffect.h"
#include "Scene.h"
#include "SpriteRenderer.h"
#include "Transform.h"

float Bubble::s_screenW = 1280.f;
float Bubble::s_screenH = 720.f;

void Bubble::OnAwake()
{
}

void Bubble::OnEnable()
{
}

void Bubble::OnStart()
{
}

void Bubble::Update(float deltaTime)
{
	if (!m_Timer || m_Timer->m_CurTime <= 0.f) return;
	if (!m_BubbleCircle) return;

	auto& input = Platform::InputSystem::Instance();
	if (!input.GetMousePressed(0)) return;

	// 마우스 스크린 좌표 → 월드 좌표 변환

	POINT p = input.GetMousePos();

	float wx = (float)p.x - s_screenW * 0.5f;
	float wy = (float)p.y - s_screenH * 0.5f;

	Vector2 pos = GetOwner()->GetTransform()->GetWorldPosition();
	float dx = wx - pos.x;
	float dy = wy - pos.y;

	if (dx * dx + dy * dy <= m_hitRadius * m_hitRadius)
	{
		if (m_BubbleCircle->IsSweetSpot())
		{
			// 정확한 타이밍에 클릭 → 공격력 +1
			if (m_Player)
			{
				m_Player->m_AttackPower++;
				LOG_DEBUG("Bubble pop! AttackPower: %d", m_Player->m_AttackPower);
			}

			// 히트 애니메이션 스폰
			if (m_BubbleHitClip)
			{
				Vector2 bubblePos = GetOwner()->GetTransform()->GetWorldPosition();

				GameObject* fxGO = GetScene()->AddObject("BubbleHit");
				fxGO->AddComponent<HitEffect>();
				fxGO->AddComponent<SpriteRenderer>();

				auto* anim = fxGO->AddComponent<Animator>();
				anim->AddClip(m_BubbleHitClip);
				anim->Play(m_BubbleHitClip->name);

				fxGO->GetTransform()->SetLocalPosition(bubblePos.x, bubblePos.y);
				fxGO->GetTransform()->SetLocalScale(1.5f, 1.5f);
			}
		}
		Destroy(GetOwner());
	}
}

void Bubble::OnDisable()
{
}

void Bubble::OnDestory()
{
}

Object* Bubble::Clone()
{
	Bubble* clone = new Bubble();
	InitClone(clone);
	return clone;
}

void Bubble::Render(ID2D1DeviceContext7* ctx)
{
}
