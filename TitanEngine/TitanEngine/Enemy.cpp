#include "pch.h"
#include "Enemy.h"

#include <random>
#include <DebugConsole/DebugConsole.h>
#include <InputSystem/InputSystem.h>

#include "GameObject.h"
#include "HitEffect.h"
#include "Scene.h"
#include "SceneManager.h"
#include "SpriteRenderer.h"
#include "Transform.h"


void Enemy::OnAwake()
{
	LOG_DEBUG("Enemy OnAwake");
}

void Enemy::OnEnable()
{
}

void Enemy::OnStart()
{
	m_Animator = GetOwner()->GetComponent<Animator>();

	m_Slider->backgroundImage.texture = m_healthBarTex;
	m_Slider->minValue = 0.f;
	m_Slider->maxValue = m_MaxHealth;
	m_Slider->SetValue(m_MaxHealth);               
	m_Slider->width = 300.f;
	m_Slider->height = 24.f;
	m_Slider->fillColor = D2D1::ColorF(0.2f, 0.85f, 0.2f, 1.f);

	LOG_DEBUG("Enemy numbers size : %d", numbers.size());

}

void Enemy::Update(float deltaTime)
{
	auto& input = Platform::InputSystem::Instance();

	if (m_Timer->m_CurTime <= 0 || m_Timer->IsGameClear())
	{
		m_Animator->SetSpeed(0);
		return;
	}

	if (input.GetMousePressed(0))
	{
		TakeDamage((float)m_Player->m_AttackPower);
	};

}

void Enemy::OnDisable()
{

}

void Enemy::OnDestory()
{

}

Object* Enemy::Clone()
{
	Enemy* clone = new Enemy(*this);
	InitClone(clone);
	return clone;
}

void Enemy::TakeDamage(float amount)
{
	m_CurrHealth -= amount;
	m_Slider->SetValue(m_CurrHealth);

	if (m_HitEffectClip)
		SpawnHitEffects();

	if (m_CurrHealth <= 0)
	{
		LOG_DEBUG("Enemy Destroy");

		m_Timer->GameClear();
		Destroy(GetOwner());
	}
	LOG_DEBUG("Enemy TakeDamage : %f ", m_CurrHealth);

}

void Enemy::SpawnHitEffects()
{
	static std::random_device rd;
	static std::default_random_engine gen(rd());
	std::uniform_int_distribution<int>    countDist(2, 4);
	std::uniform_real_distribution<float> offsetDist(-50.f, 50.f);
	std::uniform_real_distribution<float> rotateDist(0.f, 360.f);
	std::uniform_real_distribution<float> scaleDist(0.8f, 1.5f);

	int count =1; //countDist(gen);
	Vector2 enemyPos = GetOwner()->GetTransform()->GetWorldPosition();

	for (int i = 0; i < count; ++i)
	{
		float ox  = offsetDist(gen);
		float oy  = offsetDist(gen);
		float rot = rotateDist(gen);
		float sc  = scaleDist(gen);

		GameObject* fxGO = GetScene()->AddObject("HitEffect");
		fxGO->AddComponent<HitEffect>();
		auto* sr = fxGO->AddComponent<SpriteRenderer>();

		if (m_Player)
		{
			int atk = m_Player->m_AttackPower;
			if (atk >= 4)
				sr->sprite.tint = { 1.f, 0.f, 0.f, 1.f };   // 빨강
			else if (atk >= 2)
				sr->sprite.tint = { 0.5f, 0.f, 1.f, 1.f };  // 보라
		}

		auto* anim = fxGO->AddComponent<Animator>();
		anim->AddClip(m_HitEffectClip);
		anim->Play(m_HitEffectClip->name);

		fxGO->GetTransform()->SetLocalPosition(enemyPos.x + ox, enemyPos.y + oy);
		//fxGO->GetTransform()->SetLocalRotation(rot);
		fxGO->GetTransform()->SetLocalScale(sc, sc);
	}
}

void Enemy::Render(ID2D1DeviceContext7* ctx)
{

}
