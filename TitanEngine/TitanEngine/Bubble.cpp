#include "pch.h"
#include "Bubble.h"

#include <DebugConsole/DebugConsole.h>
#include <InputSystem/InputSystem.h>

#include "Animator.h"
#include "GameObject.h"
#include "HitEffect.h"
#include "JudgeText.h"
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
		// Grade the click by how tightly the ring matched the bubble, and show
		// a PERFECT/GOOD/BAD label above it so the player learns the timing.
		BubbleCircle::Timing timing = m_BubbleCircle->GetTiming();

		float         fxScale;
		D2D1_COLOR_F  fxTint;
		const wchar_t* label;
		D2D1_COLOR_F  labelColor;

		if (timing == BubbleCircle::Timing::Perfect)
		{
			if (m_Player) m_Player->OnPerfectPop();    // big power + crit chance
			fxScale    = 2.4f;
			fxTint     = { 1.0f, 0.85f, 0.1f, 1.f };   // golden burst
			label      = L"PERFECT!";
			labelColor = { 1.0f, 0.85f, 0.1f, 1.f };
		}
		else if (timing == BubbleCircle::Timing::Good)
		{
			if (m_Player) m_Player->OnGoodPop();        // solid power gain
			fxScale    = 1.6f;
			fxTint     = { 0.4f, 0.8f, 1.0f, 1.f };     // cool blue
			label      = L"GOOD";
			labelColor = { 0.45f, 0.85f, 1.0f, 1.f };
		}
		else
		{
			// Too early / too late: bubble pops, grants nothing, and breaks the combo.
			if (m_Player) m_Player->OnMissPop();
			fxScale    = 1.0f;
			fxTint     = { 0.6f, 0.6f, 0.6f, 1.f };     // dull grey
			label      = L"BAD";
			labelColor = { 1.0f, 0.30f, 0.25f, 1.f };
		}

		if (m_Player)
			LOG_DEBUG("Bubble pop! timing=%d AttackPower=%d Combo=%d",
				(int)timing, m_Player->m_AttackPower, m_Player->m_Combo);

		// Hit animation, scaled and tinted to sell the timing grade.
		if (m_BubbleHitClip)
		{
			Vector2 bubblePos = GetOwner()->GetTransform()->GetWorldPosition();

			GameObject* fxGO = GetScene()->AddObject("BubbleHit");
			fxGO->AddComponent<HitEffect>();
			auto* fxSR = fxGO->AddComponent<SpriteRenderer>();
			fxSR->sprite.tint = fxTint;

			auto* anim = fxGO->AddComponent<Animator>();
			anim->AddClip(m_BubbleHitClip);
			anim->Play(m_BubbleHitClip->name);

			fxGO->GetTransform()->SetLocalPosition(bubblePos.x, bubblePos.y);
			fxGO->GetTransform()->SetLocalScale(fxScale, fxScale);
		}

		// Floating PERFECT/GOOD/BAD label above the bubble.
		{
			Vector2 bubblePos = GetOwner()->GetTransform()->GetWorldPosition();
			GameObject* judgeGO = GetScene()->AddObject("JudgeText");
			if (judgeGO)
			{
				auto* judge = judgeGO->AddComponent<JudgeText>();
				judgeGO->GetTransform()->SetLocalPosition(bubblePos.x, bubblePos.y - 35.f);
				judge->Setup(label, labelColor);
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
