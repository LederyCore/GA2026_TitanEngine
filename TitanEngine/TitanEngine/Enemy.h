#pragma once
#include <map>
#include <memory>
#include "AnimationClip.h"
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

	std::shared_ptr<AnimationClip> m_HitEffectClip;

public:
	float m_MaxHealth = 100.0f;
	float m_CurrHealth = m_MaxHealth;

	void TakeDamage(int amount);

	std::map<char, std::shared_ptr<Texture2D>> numbers;

	std::shared_ptr<Texture2D> m_healthBarTex;
private:
	void SpawnHitEffects();
	void Render(ID2D1DeviceContext7* ctx) override;

};


