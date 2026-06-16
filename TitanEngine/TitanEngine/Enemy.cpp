#include "pch.h"
#include "Enemy.h"

#include <DebugConsole/DebugConsole.h>
#include <InputSystem/InputSystem.h>

#include "GameObject.h"
#include "SceneManager.h"


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

	m_Slider->minValue = 0.f;
	m_Slider->maxValue = m_MaxHealth;
	m_Slider->SetValue(m_MaxHealth);               
	m_Slider->width = 300.f;
	m_Slider->height = 24.f;
	m_Slider->fillColor = D2D1::ColorF(0.2f, 0.85f, 0.2f, 1.f);  // �ʷ�

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
		//float dmg = m_Player ? (float)m_Player->m_AttackPower : 1.f;
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

	if (m_CurrHealth <= 0)
	{
		LOG_DEBUG("Enemy Destroy");

		m_Timer->GameClear();
		Destroy(GetOwner());
	}
	LOG_DEBUG("Enemy OnAwake : %f ", m_CurrHealth);

}

void Enemy::Render(ID2D1DeviceContext7* ctx)
{

}
