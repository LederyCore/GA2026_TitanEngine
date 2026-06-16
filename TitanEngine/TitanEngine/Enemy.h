#pragma once
#include "Animator.h"
#include "Component.h"
#include "InGameTimer.h"
#include "Player.h"
#include "Slider.h"


using namespace TitanEngine;

class Enemy : public Component, public IUpdateable, public IRenderable
{
public:
	void OnAwake() override;
	void OnEnable() override;
	void OnStart() override;
	void Update(float deltaTime) override;
	void OnDisable() override;
	void OnDestory() override;

	
	Object* Clone() override;


public:
	Animator* m_Animator;
	Slider* m_Slider;
	InGameTimer* m_Timer;
	Player* m_Player = nullptr;

public:
	float m_MaxHealth = 100.0f;
	float m_CurrHealth = m_MaxHealth;

	void TakeDamage(float amount);

private:
	void Render(ID2D1DeviceContext7* ctx) override;

};


