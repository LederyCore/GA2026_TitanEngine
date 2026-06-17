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

	// Remember the resting transform so the hit reaction can spring back to it.
	auto* tr = GetOwner()->GetTransform();
	m_BaseScaleX = tr->GetLocalScale().x;
	m_BaseScaleY = tr->GetLocalScale().y;
	m_BasePosX   = tr->GetLocalPosition().x;
	m_BasePosY   = tr->GetLocalPosition().y;
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
		// Damage is the player's real, combo-scaled attack - no longer random.
		Player::AttackResult atk = m_Player->RollAttack();
		TakeDamage(atk.damage, atk.crit);
	}

	// Hit reaction: a quick squash + sideways knockback that springs back.
	auto* tr = GetOwner()->GetTransform();
	if (m_HitReact > 0.f)
	{
		m_HitReact -= deltaTime;
		float k     = m_HitReact / 0.18f;            // 1 -> 0 over the reaction
		if (k < 0.f) k = 0.f;
		float punch = 0.22f * k;                      // squash amount
		tr->SetLocalScale(m_BaseScaleX * (1.f - punch), m_BaseScaleY * (1.f + punch));
		tr->SetLocalPosition(m_BasePosX + 28.f * k, m_BasePosY);
	}
	else
	{
		tr->SetLocalScale(m_BaseScaleX, m_BaseScaleY);
		tr->SetLocalPosition(m_BasePosX, m_BasePosY);
	}
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

void Enemy::TakeDamage(int amount, bool crit)
{
	// Play the hit SFX on every hit (overlapping shots allowed via PlayOneShot).
	// Crits ring out a little higher; normal hits get a slight random pitch so
	// repeated hits sound varied.
	if (m_hitSfx)
	{
		static std::random_device rd;
		static std::default_random_engine gen(rd());
		std::uniform_real_distribution<float> pitchDist(0.90f, 1.12f);
		float pitch = crit ? 1.25f : pitchDist(gen);
		AudioManager::Instance().PlayOneShot(m_hitSfx, 1.0f, AudioCategory::SFX, pitch);
	}

	m_CurrHealth -= amount;
	if (m_CurrHealth < 0.f) m_CurrHealth = 0.f;
	m_Slider->SetValue(m_CurrHealth);

	// Kick off the squash/knockback reaction.
	m_HitReact = 0.18f;

	// Floating damage number - now shows the REAL damage dealt.
	// (AddObject can return null if the GameObject cap is hit during frantic
	// play, so guard the whole popup block.)
	GameObject* popup = GetScene()->AddObject("DamagePopup");
	if (popup)
	{
		// Enemy sprite is 128x64 per frame at scale 3 -> 192px tall, centered on
		// y, so the head top sits near y-96. Spawn the number just above the head,
		// using the resting position so knockback doesn't jitter the popup.
		popup->GetTransform()->SetLocalPosition(m_BasePosX + 40.f, m_BasePosY - 110);

		auto* dp = popup->AddComponent<DamagePopup>();
		popup->AddComponent<SpriteRenderer>();
		dp->numbers = numbers;
		dp->Init(std::to_string(amount));

		// Crits read bigger and gold; normal hits stay at the default look.
		if (crit)
		{
			popup->GetTransform()->SetLocalScale(1.7f, 1.7f);
			for (auto* g : dp->gos)
				g->GetComponent<SpriteRenderer>()->sprite.tint = { 1.0f, 0.82f, 0.1f, 1.f };
		}
	}

	if (m_HitEffectClip)
		SpawnHitEffects();

	if (m_CurrHealth <= 0)
	{
		LOG_DEBUG("Enemy Destroy");

		m_Timer->GameClear();
		Destroy(GetOwner());
	}
	LOG_DEBUG("Enemy TakeDamage : %f (dmg=%d crit=%d)", m_CurrHealth, amount, (int)crit);
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
