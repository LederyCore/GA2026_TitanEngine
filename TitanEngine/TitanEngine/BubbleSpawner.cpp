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

	m_NextSpawnTimer -= deltaTime;
	if (m_NextSpawnTimer <= 0.f)
	{
		SpawnBubble();
		m_NextSpawnTimer = RandomInterval();
	}
}

void BubbleSpawner::SpawnBubble()
{
	std::random_device rd;
	std::default_random_engine gen(rd());
	std::uniform_int_distribution<int> posDist(-300, 300);

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

	bubbleGO->GetTransform()->SetLocalPosition(
		(float)posDist(gen), (float)posDist(gen));

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
	return dist(gen);
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
