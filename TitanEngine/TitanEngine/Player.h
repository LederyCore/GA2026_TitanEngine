#pragma once
#include <dwrite.h>
#include <wrl/client.h>

#include "Animator.h"
#include "Component.h"
#include "InGameTimer.h"

using namespace TitanEngine;

class Player : public Component, public IUpdateable, public IRenderable
{
public:
	void OnAwake() override;
	void OnEnable() override;
	void OnStart() override;
	void Update(float deltaTime) override;
	void OnDisable() override;
	void OnDestory() override;

	Object* Clone() override;

	// Real client size, pushed in by Engine each frame (same pattern as Button/Bubble).
	// Used so the HUD matches the scene's screen transform instead of guessing.
	static void SetScreenSize(float w, float h) { s_screenW = w; s_screenH = h; }

private:
	void Render(ID2D1DeviceContext7* ctx) override;

public:
	Animator* m_Animator;
	int animCnt = 0;

	InGameTimer* m_Timer;

	// --- Big-number stats (start at 4 digits, ramp toward 6 digits) ---
	int   m_AttackPower = 1200;   // 4-digit base damage for instant dopamine
	int   m_Combo       = 0;      // consecutive successful bubble pops
	int   m_BestCombo   = 0;      // highest combo reached this match
	int   m_HitCount    = 0;      // total enemy swings landed
	float m_CritChance  = 0.05f;  // 0..1, grows as you pop bubbles
	float m_CritMult    = 2.5f;   // crit damage multiplier

	float m_ComboPunch  = 0.f;    // brief scale-up pulse when the combo ticks up

	static const int kMaxDamage = 999999; // hard cap = 6 digits

	// Result of one attack swing against the enemy.
	struct AttackResult { int damage; bool crit; };
	AttackResult RollAttack();

	// Reward hooks invoked by Bubble when a bubble is clicked.
	void OnPerfectPop(); // dead-on timing: big power + crit chance
	void OnGoodPop();    // close timing: small power + crit chance
	void OnMissPop();    // mistimed click: combo breaks

private:
	// HUD (DirectWrite): bottom-left stats panel + left-middle combo counter.
	void EnsureHudResources(ID2D1DeviceContext7* ctx);
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_hudBrush;
	Microsoft::WRL::ComPtr<IDWriteFactory>       m_dwriteFactory;
	Microsoft::WRL::ComPtr<IDWriteTextFormat>    m_hudFormat;
	Microsoft::WRL::ComPtr<IDWriteTextFormat>    m_comboFormat;

	static float s_screenW;
	static float s_screenH;
};

