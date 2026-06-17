#include "pch.h"
#include "JudgeText.h"

#include "GameObject.h"
#include "Transform.h"

float JudgeText::s_screenW = 1080.f;
float JudgeText::s_screenH = 720.f;

Object* JudgeText::Clone()
{
	return nullptr;
}

void JudgeText::Setup(const wchar_t* label, D2D1_COLOR_F color)
{
	m_label = label ? label : L"";
	m_color = color;
}

void JudgeText::Update(float deltaTime)
{
	// Drift upward, then self-destruct when the lifetime runs out.
	auto* tr = GetOwner()->GetTransform();
	Vector2 p = tr->GetLocalPosition();
	p.y -= deltaTime * 90.f;
	tr->SetLocalPosition(p);

	m_life -= deltaTime;
	if (m_life <= 0.f)
		Destroy(GetOwner());
}

void JudgeText::EnsureResources(ID2D1DeviceContext7* ctx)
{
	if (!m_brush)
		ctx->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_brush);

	if (!m_format)
	{
		if (!m_dwrite)
			DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
				__uuidof(IDWriteFactory),
				reinterpret_cast<IUnknown**>(m_dwrite.GetAddressOf()));

		if (m_dwrite)
			m_dwrite->CreateTextFormat(
				L"Segoe UI", nullptr,
				DWRITE_FONT_WEIGHT_BLACK,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				26.f, L"en-US",
				&m_format);

		if (m_format)
		{
			m_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			m_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		}
	}
}

void JudgeText::Render(ID2D1DeviceContext7* ctx)
{
	EnsureResources(ctx);
	if (!m_format || !m_brush || m_label.empty())
		return;

	// Root object, so local position == world position. Using local avoids a
	// one-frame glitch on the spawn frame (world matrix isn't propagated yet).
	Vector2 wp = GetOwner()->GetTransform()->GetLocalPosition();

	// Match the scene's screen transform exactly (origin at screen center).
	D2D1::Matrix3x2F screenT =
		D2D1::Matrix3x2F::Translation(s_screenW * 0.5f, s_screenH * 0.5f);

	// Quick pop-in: scale ~1.4 -> 1.0 over the first slice of life.
	float t   = 1.f - (m_life / m_maxLife);   // 0..1
	float pop = (t < 0.25f) ? (1.4f - 1.6f * t) : 1.0f;
	if (pop < 1.0f) pop = 1.0f;

	ctx->SetTransform(D2D1::Matrix3x2F::Scale(pop, pop, D2D1::Point2F(wp.x, wp.y)) * screenT);

	float alpha = m_life / m_maxLife;
	if (alpha > 1.f) alpha = 1.f;
	D2D1_COLOR_F c = m_color;
	c.a = alpha;
	m_brush->SetColor(c);

	D2D1_RECT_F rect = { wp.x - 120.f, wp.y - 30.f, wp.x + 120.f, wp.y + 30.f };
	ctx->DrawText(m_label.c_str(), (UINT32)m_label.size(), m_format.Get(), rect, m_brush.Get());

	ctx->SetTransform(screenT);
}
