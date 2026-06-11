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
}

void Enemy::Update(float deltaTime)
{
	auto& input = Platform::InputSystem::Instance();

	if (input.GetMousePressed(0))
	{
		TakeDamage(10.f);
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

	if (m_CurrHealth <= 0)
	{
		LOG_DEBUG("Enemy Destroy");

		//Destroy(GetOwner());
	}
	LOG_DEBUG("Enemy OnAwake : %f ", m_CurrHealth);

}

void Enemy::Render(ID2D1DeviceContext7* ctx)
{

}
