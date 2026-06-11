#include "pch.h"
#include "Enemy.h"

#include <DebugConsole/DebugConsole.h>
#include <InputSystem/InputSystem.h>

#include "SceneManager.h"

//using namespace TitanEngine;

void TitanEngine::Enemy::OnAwake()
{
	LOG_DEBUG("Enemy OnAwake");

}

void TitanEngine::Enemy::OnEnable()
{
}

void TitanEngine::Enemy::OnStart()
{
}

void TitanEngine::Enemy::Update(float deltaTime)
{
	auto& input = Platform::InputSystem::Instance();

	if (input.GetMousePressed(0))
	{
		TakeDamage(10.f);
	};

	if (input.GetKeyPressed(VK_SPACE))
	{
		SceneManagement::SceneManager::Instance().LoadScene("TestScene");
	}
}

void TitanEngine::Enemy::OnDisable()
{

}

void TitanEngine::Enemy::OnDestory()
{

}

TitanEngine::Object* TitanEngine::Enemy::Clone()
{
	Enemy* clone = new Enemy(*this);
	InitClone(clone);
	return clone;
}

void TitanEngine::Enemy::TakeDamage(float amount)
{
	m_CurrHealth -= amount;

	if (m_CurrHealth <= 0)
	{
		LOG_DEBUG("Enemy Destroy");

		//Destroy(this);
	}
	LOG_DEBUG("Enemy OnAwake : %f ", m_CurrHealth);

}

void TitanEngine::Enemy::Render(ID2D1DeviceContext7* ctx)
{

}
