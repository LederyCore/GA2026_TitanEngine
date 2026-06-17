#include "pch.h"
#include "BubbleSpawner.h"

#include <random>
#include <DebugConsole/DebugConsole.h>

#include "Bubble.h"
#include "BubbleCircle.h"
#include "GameObject.h"
#include "Scene.h"
#include "SpriteRenderer.h"
#include "Transform.h"

void BubbleSpawner::OnAwake()
{
}

void BubbleSpawner::OnEnable()
{
}

void BubbleSpawner::OnStart()
{
	m_NextSpawnTimer = RandomInterval();
}

void BubbleSpawner::Update(float deltaTime)
{
	if (!m_Timer || m_Timer->m_CurTime <= 0.f) return;
	if (m_Timer->IsGameClear()) return;
	m_NextSpawnTimer -= deltaTime;
	if (m_NextSpawnTimer <= 0.f)
	{
		SpawnBubble();

		// The match gets more frantic as time runs out: past the halfway
		// point each spawn pulse drops an extra bubble.
		if (GetProgress() > 0.5f)
			SpawnBubble();

		m_NextSpawnTimer = RandomInterval();
	}
}

void BubbleSpawner::SpawnBubble()
{
	std::random_device rd;
	std::default_random_engine gen(rd());
	std::uniform_int_distribution<int> posDist(-300, 300);

	// Pick a spot that isn't on top of the player/enemy art. Rejection-sample a
	// handful of times; fall back to the last roll if everything is blocked.
	float px = 0.f, py = 0.f;
	for (int attempt = 0; attempt < 30; ++attempt)
	{
		px = (float)posDist(gen);
		py = (float)posDist(gen);
		if (!IsBlocked(px, py))
			break;
	}

	// BubbleCircle GameObject (Bubble의 자식)
	GameObject* circleGO = GetScene()->AddObject("BubbleCircle");
	auto* circleComp = circleGO->AddComponent<BubbleCircle>();
	auto* circleSR   = circleGO->AddComponent<SpriteRenderer>();
	circleComp->m_Timer = m_Timer;

	// Bubble GameObject
	GameObject* bubbleGO = GetScene()->AddObject("Bubble");
	auto* bubbleComp = bubbleGO->AddComponent<Bubble>();
	auto* bubbleSR   = bubbleGO->AddComponent<SpriteRenderer>();

	if (m_BubbleTex)
		bubbleSR->sprite.texture = m_BubbleTex;

	bubbleGO->GetTransform()->SetLocalPosition(px, py);

	bubbleComp->m_Player        = m_Player;
	bubbleComp->m_Timer         = m_Timer;
	bubbleComp->m_BubbleHitClip = m_BubbleHitClip;


	if (m_CircleTex)
		circleSR->sprite.texture = m_CircleTex;

	circleGO->GetTransform()->SetParent(bubbleGO->GetTransform());
	circleGO->GetTransform()->SetLocalPosition(0.f, 0.f);

	bubbleComp->m_BubbleCircle = circleComp;

	LOG_DEBUG("BubbleSpawner: bubble spawned");
}

float BubbleSpawner::RandomInterval()
{
	std::random_device rd;
	std::default_random_engine gen(rd());
	std::uniform_real_distribution<float> dist(m_MinInterval, m_MaxInterval);

	// Bubbles come up to ~65% faster by the end of the match.
	float scale = 1.0f - 0.65f * GetProgress();
	float v     = dist(gen) * scale;
	return v < 0.15f ? 0.15f : v;
}

float BubbleSpawner::GetProgress() const
{
	if (!m_Timer || m_Timer->m_MaxTime <= 0.f)
		return 0.f;
	float p = 1.f - m_Timer->m_CurTime / m_Timer->m_MaxTime;
	if (p < 0.f) return 0.f;
	if (p > 1.f) return 1.f;
	return p;
}

void BubbleSpawner::AddExclusionRect(float cx, float cy, float halfW, float halfH, float margin)
{
	m_Exclusions.push_back({
		cx - halfW - margin, cy - halfH - margin,
		cx + halfW + margin, cy + halfH + margin });
}

bool BubbleSpawner::IsBlocked(float x, float y) const
{
	for (const auto& z : m_Exclusions)
	{
		if (x >= z.minX && x <= z.maxX && y >= z.minY && y <= z.maxY)
			return true;
	}
	return false;
}

void BubbleSpawner::OnDisable()
{
}

void BubbleSpawner::OnDestory()
{
}

Object* BubbleSpawner::Clone()
{
	return nullptr;
}
