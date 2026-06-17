#include "pch.h"
#include "Player.h"

#include <cstdio>
#include <random>
#include <InputSystem/InputSystem.h>

#include "GameObject.h"

float Player::s_screenW = 1080.f;
float Player::s_screenH = 720.f;

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

	// Combo punch pulse decays every frame (even after the swing animation).
	if (m_ComboPunch > 0.f)
		m_ComboPunch -= deltaTime;

	if (m_Timer->m_CurTime <= 0 || m_Timer->IsGameClear())
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

Player::AttackResult Player::RollAttack()
{
	m_HitCount++;

	static std::random_device rd;
	static std::default_random_engine gen(rd());
	static std::uniform_real_distribution<float> roll01(0.f, 1.f);
	// Damage spreads around the attack power: e.g. ATK 1200 -> ~984..1404.
	static std::uniform_real_distribution<float> spread(0.82f, 1.17f);

	// Base = attack power +- a random spread, then scaled by the combo (+10%/combo).
	float    comboMul = 1.0f + m_Combo * 0.10f;
	long long dmg     = (long long)(m_AttackPower * spread(gen) * comboMul);

	// Crit is a real probability the player grows by popping bubbles.
	bool crit = roll01(gen) < m_CritChance;
	if (crit)
		dmg = (long long)(dmg * m_CritMult);

	// Cap at 6 digits; never below 1.
	if (dmg > kMaxDamage) dmg = kMaxDamage;
	if (dmg < 1)          dmg = 1;

	return { (int)dmg, crit };
}

void Player::OnPerfectPop()
{
	m_AttackPower += 900;                 // big power spike
	m_Combo++;
	if (m_Combo > m_BestCombo) m_BestCombo = m_Combo;

	m_CritChance += 0.06f;                // crit chance climbs fast
	if (m_CritChance > 1.f) m_CritChance = 1.f;
	m_CritMult += 0.05f;                  // and crits hit a bit harder

	m_ComboPunch = 0.28f;                 // pop the combo counter
}

void Player::OnGoodPop()
{
	m_AttackPower += 300;                 // small power gain
	m_Combo++;
	if (m_Combo > m_BestCombo) m_BestCombo = m_Combo;

	m_CritChance += 0.025f;
	if (m_CritChance > 1.f) m_CritChance = 1.f;

	m_ComboPunch = 0.28f;                 // pop the combo counter
}

void Player::OnMissPop()
{
	// A mistimed bubble click snaps the combo back to zero.
	m_Combo = 0;
}

void Player::EnsureHudResources(ID2D1DeviceContext7* ctx)
{
	if (!m_hudBrush)
		ctx->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_hudBrush);

	if (!m_hudFormat)
	{
		if (!m_dwriteFactory)
			DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
				__uuidof(IDWriteFactory),
				reinterpret_cast<IUnknown**>(m_dwriteFactory.GetAddressOf()));

		if (m_dwriteFactory)
			m_dwriteFactory->CreateTextFormat(
				L"Segoe UI", nullptr,
				DWRITE_FONT_WEIGHT_BOLD,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				20.f, L"ko-KR",
				&m_hudFormat);

		if (m_hudFormat)
		{
			m_hudFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
			m_hudFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		}
	}

	if (!m_comboFormat && m_dwriteFactory)
	{
		m_dwriteFactory->CreateTextFormat(
			L"Segoe UI", nullptr,
			DWRITE_FONT_WEIGHT_BLACK,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			34.f, L"ko-KR",
			&m_comboFormat);

		if (m_comboFormat)
		{
			m_comboFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			m_comboFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		}
	}
}

void Player::Render(ID2D1DeviceContext7* ctx)
{
	EnsureHudResources(ctx);
	if (!m_hudFormat || !m_hudBrush)
		return;

	// Use the REAL client size (pushed in by Engine) so this exactly matches the
	// scene's screen transform. Hardcoding 1280x720 here shifted every object
	// drawn after the player (timer bar, buttons) to the right.
	const float screenW = s_screenW;
	const float screenH = s_screenH;

	const D2D1::Matrix3x2F screenT =
		D2D1::Matrix3x2F::Translation(screenW * 0.5f, screenH * 0.5f);
	ctx->SetTransform(screenT);

	// Bottom-left panel.
	D2D1_RECT_F panel = {
		-screenW * 0.5f + 16.f,
		 screenH * 0.5f - 140.f,
		-screenW * 0.5f + 330.f,
		 screenH * 0.5f - 14.f
	};

	// Dim backdrop so the text stays readable over the background art.
	m_hudBrush->SetColor(D2D1::ColorF(0.f, 0.f, 0.f, 0.45f));
	ctx->FillRectangle(panel, m_hudBrush.Get());

	// HUD lines: hit count / attack power / crit chance / crit damage.
	// NOTE: this file is saved as UTF-8 *with BOM* so MSVC reads the Korean
	// string literals below correctly (the project does not pass /utf-8).
	wchar_t buf[256];
	int len = swprintf_s(buf, 256,
		L"타격수 : %d\n"
		L"공격력 : %d\n"
		L"크리티컬 확률 : %.0f%%\n"
		L"크리티컬 데미지 : %.0f%%",
		m_HitCount,
		m_AttackPower,
		m_CritChance * 100.f,
		m_CritMult   * 100.f);

	D2D1_RECT_F textRect = { panel.left + 12.f, panel.top + 8.f, panel.right - 8.f, panel.bottom - 6.f };
	m_hudBrush->SetColor(D2D1::ColorF(1.f, 0.95f, 0.4f, 1.f)); // warm gold
	ctx->DrawText(buf, (UINT32)(len < 0 ? 0 : len), m_hudFormat.Get(), textRect, m_hudBrush.Get());

	// --- Combo counter: left edge, middle height, with a punch on each tick ---
	if (m_Combo >= 2 && m_comboFormat)
	{
		const float cx = -screenW * 0.5f + 130.f; // a bit in from the left edge
		const float cy = 0.f;                      // vertical middle

		// Punch: snaps up to ~1.7x the instant the combo grows, settles to 1.0.
		float p     = (m_ComboPunch > 0.f) ? (m_ComboPunch / 0.28f) : 0.f;
		float scale = 1.0f + 0.7f * p;

		// Hotter color as the streak climbs: yellow -> orange -> red.
		float heat = m_Combo / 30.f;
		if (heat > 1.f) heat = 1.f;
		D2D1_COLOR_F comboColor = { 1.0f, 0.85f - 0.65f * heat, 0.15f, 1.f };

		ctx->SetTransform(D2D1::Matrix3x2F::Scale(scale, scale, D2D1::Point2F(cx, cy)) * screenT);

		wchar_t cbuf[64];
		int clen = swprintf_s(cbuf, 64, L"%d\nCOMBO", m_Combo);

		D2D1_RECT_F comboRect = { cx - 150.f, cy - 60.f, cx + 150.f, cy + 60.f };
		m_hudBrush->SetColor(comboColor);
		ctx->DrawText(cbuf, (UINT32)(clen < 0 ? 0 : clen), m_comboFormat.Get(), comboRect, m_hudBrush.Get());
	}

	// Leave the transform exactly where the scene expects it for later objects.
	ctx->SetTransform(screenT);
}
