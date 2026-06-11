#include "pch.h"
#include "Button.h"
#include "GameObject.h"
#include "Transform.h"
#include "SpriteRenderer.h"
#include "InputSystem/Input.h"
#include "InputSystem/InputSystem.h"
#pragma comment(lib, "dxguid.lib")

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
        clone->image         = image;
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
        const auto& world = tf->GetWorldMatrix();

        D2D1::Matrix3x2F screenTransform;
        ctx->GetTransform(&screenTransform);

        D2D1::Matrix3x2F d2dMatrix(
            world._11, world._12,
            world._21, world._22,
            world._41, world._42
        );

        if (image.GetBitmap())
        {
            // --- Sprite mode (same logic as SpriteRenderer) ---
            float drawW = (float)image.GetWidth();
            float drawH = (float)image.GetHeight();
            float pw    = drawW * image.pivot.x;
            float ph    = drawH * image.pivot.y;

            // apply tint to image
            image.tint = m_currentTint;

            const D2D1_COLOR_F& t     = image.tint;
            const bool needsTint = (t.r != 1.f || t.g != 1.f || t.b != 1.f || t.a != 1.f);
            const bool hasSrc    = image.srcRect.has_value();

            if (needsTint)
            {
                D2D1::Matrix3x2F finalMatrix = hasSrc
                    ? D2D1::Matrix3x2F::Translation(-pw - image.srcRect->left, -ph - image.srcRect->top) * d2dMatrix * screenTransform
                    : D2D1::Matrix3x2F::Translation(-pw, -ph) * d2dMatrix * screenTransform;
                ctx->SetTransform(finalMatrix);

                if (!m_colorEffect)
                    ctx->CreateEffect(CLSID_D2D1ColorMatrix, &m_colorEffect);

                D2D1_MATRIX_5X4_F mat = {};
                mat.m[0][0] = t.r;
                mat.m[1][1] = t.g;
                mat.m[2][2] = t.b;
                mat.m[3][3] = t.a;
                m_colorEffect->SetValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX, mat);

                Microsoft::WRL::ComPtr<ID2D1Image> bitmapImage;
                image.GetBitmap()->QueryInterface(IID_PPV_ARGS(&bitmapImage));
                m_colorEffect->SetInput(0, bitmapImage.Get());

                Microsoft::WRL::ComPtr<ID2D1Image> effectImage;
                m_colorEffect.As(&effectImage);
                if (hasSrc)
                {
                    D2D1_RECT_F srcRectF = image.srcRect.value();
                    ctx->DrawImage(effectImage.Get(), nullptr, &srcRectF,
                        D2D1_INTERPOLATION_MODE_LINEAR, D2D1_COMPOSITE_MODE_SOURCE_OVER);
                }
                else
                {
                    ctx->DrawImage(effectImage.Get(), nullptr, nullptr,
                        D2D1_INTERPOLATION_MODE_LINEAR, D2D1_COMPOSITE_MODE_SOURCE_OVER);
                }
            }
            else
            {
                D2D1::Matrix3x2F finalMatrix =
                    D2D1::Matrix3x2F::Translation(-pw, -ph) * d2dMatrix * screenTransform;
                ctx->SetTransform(finalMatrix);

                D2D1_RECT_F dstRect = { 0.f, 0.f, drawW, drawH };
                if (hasSrc)
                {
                    D2D1_RECT_F srcRectF = image.srcRect.value();
                    ctx->DrawBitmap(image.GetBitmap(), &dstRect, t.a,
                        D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &srcRectF);
                }
                else
                {
                    ctx->DrawBitmap(image.GetBitmap(), &dstRect, t.a,
                        D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
            }

            ctx->SetTransform(screenTransform);
        }
        else
        {
            // --- Plain rect mode (original behavior) ---
            Vector2 wp = tf->GetWorldPosition();
            float hw = width  * 0.5f;
            float hh = height * 0.5f;
            D2D1_RECT_F rect = { wp.x - hw, wp.y - hh, wp.x + hw, wp.y + hh };

            const D2D1_COLOR_F& c = m_currentTint;
            m_brush->SetColor(c);
            ctx->FillRectangle(rect, m_brush.Get());

            D2D1_COLOR_F borderColor = { c.r * 0.6f, c.g * 0.6f, c.b * 0.6f, c.a };
            m_brush->SetColor(borderColor);
            ctx->DrawRectangle(rect, m_brush.Get(), 1.f);
        }

        // --- Text overlay (both modes) ---
        if (!text.empty() && m_textFormat)
        {
            Vector2 wp = tf->GetWorldPosition();
            float hw = width  * 0.5f;
            float hh = height * 0.5f;
            D2D1_RECT_F rect = { wp.x - hw, wp.y - hh, wp.x + hw, wp.y + hh };

            ctx->SetTransform(screenTransform);
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

        float w = image.GetBitmap() ? (float)image.GetWidth()  : width;
        float h = image.GetBitmap() ? (float)image.GetHeight() : height;
        float hw = w * 0.5f;
        float hh = h * 0.5f;
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
