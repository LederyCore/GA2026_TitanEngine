#pragma once
#include <map>
#include <memory>
#include "AnimationClip.h"
#include "Animator.h"
#include "AudioClip.h"
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
	// Big HP pool to match the 4-6 digit damage numbers. Easy to tune here.
	float m_MaxHealth = 2500000.0f;
	float m_CurrHealth = m_MaxHealth;

	void TakeDamage(int amount, bool crit);

	// --- Hit reaction (squash + knockback punch on every hit) ---
	float m_HitReact   = 0.f;        // counts down after each hit
	float m_BaseScaleX = -3.f;       // captured in OnStart from the transform
	float m_BaseScaleY =  3.f;
	float m_BasePosX   = 100.f;
	float m_BasePosY   =  0.f;

	std::map<char, std::shared_ptr<Texture2D>> numbers;

	std::shared_ptr<Texture2D> m_healthBarTex;

	// Hit sound effect, played once on every TakeDamage (injected by the scene).
	std::shared_ptr<AudioClip> m_hitSfx;
private:
	void SpawnHitEffects();
	void Render(ID2D1DeviceContext7* ctx) override;

};


