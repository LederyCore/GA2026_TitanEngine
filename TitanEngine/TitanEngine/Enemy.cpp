#include "pch.h"
#include "Enemy.h"

#include <random>
#include <DebugConsole/DebugConsole.h>
#include <InputSystem/InputSystem.h>

#include "DamagePopup.h"
#include "GameObject.h"
#include "HitEffect.h"
#include "Scene.h"
#include "SceneManager.h"
#include "SpriteRenderer.h"
#include "Transform.h"
#include "AudioManager.h"

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
	m_Slider->SetValue(m_MaxHealth);

	// Background = full HPbar_BG frame (art is 282x60), kept at its aspect ratio.
	m_Slider->width = 300.f;
	m_Slider->height = 64.f;                 // 300 * (60/282) ~= 64, avoids vertical squish
	m_Slider->pivot = { 0.f, 0.5f };        // left-center anchor
	m_Slider->offset = { 0.f, 0.f };

	// Confine the green fill to the inner recessed channel of the frame,
	// instead of letting it cover the whole image.
	// Measured channel in the 282x60 art: x[21..258], y[20..50].
	// Scaled into the 300x64 box; y is shifted up by height/2 because of the
	// left-center pivot, so the fill box origin lands inside the channel.
	m_Slider->fillRect.pivot = { 0.f, 0.f };
	m_Slider->fillRect.offset = { 22.f, -9.f };
	m_Slider->fillRect.width = 252.f;
	m_Slider->fillRect.height = 24.f;
	m_Slider->direction = Slider::Direction::LeftToRight;
	m_Slider->fillColor = D2D1::ColorF(1.0f, 0.25f, 0.2f, 1.f);
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

void Enemy::TakeDamage(int amount)
{
	// Play the hit SFX on every hit (overlapping shots allowed via PlayOneShot).
// Slightly randomize the pitch each time so repeated hits sound varied.
	if (m_hitSfx)
	{
		static std::random_device rd;
		static std::default_random_engine gen(rd());
		std::uniform_real_distribution<float> pitchDist(0.90f, 1.12f);
		AudioManager::Instance().PlayOneShot(m_hitSfx, 1.0f, AudioCategory::SFX, pitchDist(gen));
	}

	m_CurrHealth -= amount;
	m_Slider->SetValue(m_CurrHealth);

	GameObject* popup = GetScene()->AddObject("DamagePopup");
	auto p = GetOwner()->GetTransform()->GetLocalPosition();
	popup->GetTransform()->SetLocalPosition(p.x, p.y - 80);

	popup->AddComponent<DamagePopup>();
	popup->AddComponent<SpriteRenderer>();
	popup->GetComponent<DamagePopup>()->numbers = numbers;

	static std::random_device rd;
	static std::default_random_engine gen(rd());
	std::uniform_int_distribution<int>    countDist(1, 100000);

	amount = countDist(gen);
	popup->GetComponent<DamagePopup>()->Init(std::to_string(amount));
	//popup->GetComponent<SpriteRenderer>()->sprite.texture = numbers[0];

	

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
