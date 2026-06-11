#pragma once
#include "Animator.h"
#include "Component.h"
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

	// Component을(를) 통해 상속됨
	Object* Clone() override;


public:
	Animator* m_Animator;
	Slider* m_Slider;
public:
	float m_MaxHealth = 100.0f;
	float m_CurrHealth = m_MaxHealth;

	void TakeDamage(float amount);

private:
	// IRenderable을(를) 통해 상속됨
	void Render(ID2D1DeviceContext7* ctx) override;

};


