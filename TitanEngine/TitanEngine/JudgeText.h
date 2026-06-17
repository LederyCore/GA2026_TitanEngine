#pragma once
#include <string>
#include <d2d1_3.h>
#include <dwrite.h>
#include <wrl/client.h>

#include "Component.h"

using namespace TitanEngine;

// Floating "PERFECT!/GOOD/BAD" judgement that pops above a clicked bubble,
// drifts upward and fades out. ASCII-only labels, so this file needs no BOM.
class JudgeText : public Component, public IUpdateable, public IRenderable
{
public:
	Object* Clone() override;
	void Update(float deltaTime) override;
	void Render(ID2D1DeviceContext7* ctx) override;

	// Set the text and color right after AddComponent.
	void Setup(const wchar_t* label, D2D1_COLOR_F color);

	// Real client size pushed in by Engine (same pattern as Button/Bubble/Player).
	static void SetScreenSize(float w, float h) { s_screenW = w; s_screenH = h; }

private:
	void EnsureResources(ID2D1DeviceContext7* ctx);

	std::wstring m_label;
	D2D1_COLOR_F m_color   = { 1.f, 1.f, 1.f, 1.f };
	float        m_life    = 0.6f;
	float        m_maxLife = 0.6f;

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_brush;
	Microsoft::WRL::ComPtr<IDWriteFactory>       m_dwrite;
	Microsoft::WRL::ComPtr<IDWriteTextFormat>    m_format;

	static float s_screenW;
	static float s_screenH;
};
