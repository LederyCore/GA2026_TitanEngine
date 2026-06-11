#include "pch.h"
#include "Button.h"
#include "GameObject.h"
#include "Transform.h"
#include "SpriteRenderer.h"
#include "InputSystem/Input.h"
#include "InputSystem/InputSystem.h"

namespace TitanEngine
{
    float Button::s_screenW = 1280.f;
    float Button::s_screenH = 720.f;

    Button::ColorBlock::ColorBlock()
    {
        normalColor      = D2D1::ColorF(1.f,   1.f,   1.f,  1.f );
        highlightedColor = D2D1::ColorF(0.96f, 0.96f, 0.96f,1.f );
        pressedColor     = D2D1::ColorF(0.78f, 0.78f, 0.78f,1.f );
        disabledColor    = D2D1::ColorF(0.78f, 0.78f, 0.78f,0.5f);
    }

    void Button::OnAwake()
    {
        m_currentTint = colors.normalColor;
        m_fromTint    = colors.normalColor;
        textColor     = D2D1::ColorF(0.2f, 0.2f, 0.2f, 1.f);
    }

    Object* Button::Clone()
    {
        auto* clone = new Button();
        InitClone(clone);
        clone->onClick       = onClick;
        clone->interactable  = interactable;
        clone->colors        = colors;
        clone->width         = width;
        clone->height        = height;
        clone->text          = text;
        clone->textColor     = textColor;
        clone->fontSize      = fontSize;
        clone->targetGraphic = nullptr;
        return clone;
    }

    void Button::Update(float deltaTime)
    {
        POINT mp   = Input::GetMousePosition();
        bool  over = IsPointInBounds((float)mp.x, (float)mp.y);

        SelectionState next = SelectionState::Normal;

        if (!interactable)
        {
            next = SelectionState::Disabled;
        }
        else if (over)
        {
            if (Input::GetMouseButton(0))
                next = SelectionState::Pressed;
            else
                next = SelectionState::Highlighted;

            if (m_wasPressed && Input::GetMouseButtonUp(0))
            {
                if (onClick)
                    onClick();
            }
        }

        m_wasPressed = over && Input::GetMouseButton(0);

        if (next != m_state)
            StartTransition(next);

        if (m_fadeTimer < colors.fadeDuration)
        {
            m_fadeTimer += deltaTime;
            float t = (colors.fadeDuration > 0.f)
                      ? (m_fadeTimer / colors.fadeDuration)
                      : 1.f;
            if (t > 1.f) t = 1.f;

            m_currentTint = LerpColor(m_fromTint, GetTargetColor(), t);
            ApplyTint(m_currentTint);
        }
    }

    void Button::Render(ID2D1DeviceContext7* ctx)
    {
        if (targetGraphic) return;

        EnsureResources(ctx);

        Transform* tf = GetOwner()->GetTransform();
        Vector2    wp = tf->GetWorldPosition();

        D2D1::Matrix3x2F screenTransform;
        ctx->GetTransform(&screenTransform);

        float hw = width  * 0.5f;
        float hh = height * 0.5f;
        D2D1_RECT_F rect = { wp.x - hw, wp.y - hh, wp.x + hw, wp.y + hh };

        const D2D1_COLOR_F& c = m_currentTint;
        m_brush->SetColor(c);
        ctx->FillRectangle(rect, m_brush.Get());

        D2D1_COLOR_F borderColor = { c.r * 0.6f, c.g * 0.6f, c.b * 0.6f, c.a };
        m_brush->SetColor(borderColor);
        ctx->DrawRectangle(rect, m_brush.Get(), 1.f);

        if (!text.empty() && m_textFormat)
        {
            m_textBrush->SetColor(textColor);
            ctx->DrawText(
                text.c_str(),
                (UINT32)text.size(),
                m_textFormat.Get(),
                rect,
                m_textBrush.Get()
            );
        }
    }

    bool Button::IsPointInBounds(float px, float py)
    {
        Transform* tf = GetOwner()->GetTransform();
        Vector2    wp = tf->GetWorldPosition();

        float wx = px - s_screenW * 0.5f;
        float wy = py - s_screenH * 0.5f;

        float hw = width  * 0.5f;
        float hh = height * 0.5f;
        return (wx >= wp.x - hw && wx <= wp.x + hw &&
                wy >= wp.y - hh && wy <= wp.y + hh);
    }

    D2D1_COLOR_F Button::GetTargetColor() const
    {
        D2D1_COLOR_F base;
        switch (m_state)
        {
        case SelectionState::Highlighted: base = colors.highlightedColor; break;
        case SelectionState::Pressed:     base = colors.pressedColor;     break;
        case SelectionState::Disabled:    base = colors.disabledColor;    break;
        default:                          base = colors.normalColor;      break;
        }
        return MultiplyColor(base, colors.colorMultiplier);
    }

    void Button::StartTransition(SelectionState newState)
    {
        m_fromTint  = m_currentTint;
        m_state     = newState;
        m_fadeTimer = 0.f;
    }

    void Button::ApplyTint(const D2D1_COLOR_F& tint)
    {
        if (!targetGraphic) return;
        targetGraphic->sprite.tint = tint;
    }

    void Button::EnsureResources(ID2D1DeviceContext7* ctx)
    {
        if (!m_brush)
            ctx->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_brush);

        if (!m_textBrush)
            ctx->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_textBrush);

        if (!m_textFormat)
        {
            if (!m_dwriteFactory)
                DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
                    __uuidof(IDWriteFactory),
                    reinterpret_cast<IUnknown**>(m_dwriteFactory.GetAddressOf()));

            if (m_dwriteFactory)
                m_dwriteFactory->CreateTextFormat(
                    L"Segoe UI", nullptr,
                    DWRITE_FONT_WEIGHT_NORMAL,
                    DWRITE_FONT_STYLE_NORMAL,
                    DWRITE_FONT_STRETCH_NORMAL,
                    fontSize, L"ko-KR",
                    &m_textFormat);

            if (m_textFormat)
            {
                m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            }
        }
    }

    D2D1_COLOR_F Button::LerpColor(const D2D1_COLOR_F& a, const D2D1_COLOR_F& b, float t)
    {
        return { a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t,
                 a.b + (b.b - a.b) * t, a.a + (b.a - a.a) * t };
    }

    D2D1_COLOR_F Button::MultiplyColor(const D2D1_COLOR_F& c, float mult)
    {
        return { c.r * mult, c.g * mult, c.b * mult, c.a };
    }
}
