#include "pch.h"
#include "Player.h"

#include <InputSystem/InputSystem.h>

#include "GameObject.h"

void Player::OnAwake()
{
}

void Player::OnEnable()
{
}

void Player::OnStart()
{
	m_Animator = GetOwner()->GetComponent<Animator>();
}

void Player::Update(float deltaTime)
{
	auto& input = Platform::InputSystem::Instance();

	if (m_Timer->m_CurTime <= 0)
	{
		m_Animator->SetSpeed(0);
		return;
	}

	if (input.GetMousePressed(0))
	{
		if (animCnt == 0)
		{
			m_Animator->Play("player_attack1");
			animCnt++;
		}
		else if (animCnt == 1)
		{
			m_Animator->Play("player_attack2");
			animCnt++;
		}
		else
		{
			m_Animator->Play("player_attack3");
			animCnt = 0;
		}
	};
}

void Player::OnDisable()
{
}

void Player::OnDestory()
{
}

Object* Player::Clone()
{
	return nullptr;
}

void Player::Render(ID2D1DeviceContext7* ctx)
{
}
