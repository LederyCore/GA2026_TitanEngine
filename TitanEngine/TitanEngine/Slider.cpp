#include "pch.h"
#include "Slider.h"
#include "GameObject.h"
#include "Transform.h"
#pragma comment(lib, "dxguid.lib")

namespace TitanEngine
{
    // ---------------------------------------------------------------------------
    // API
    // ---------------------------------------------------------------------------

    void Slider::SetValue(float v)
    {
        float clamped = v < minValue ? minValue : (v > maxValue ? maxValue : v);
        if (clamped == m_value) return;
        m_value = clamped;
        if (onValueChanged) onValueChanged(m_value);
    }

    float Slider::GetNormalizedValue() const
    {
        float range = maxValue - minValue;
        if (range <= 0.f) return 0.f;
        return (m_value - minValue) / range;
    }

    void Slider::OnAwake()
    {
        m_value           = maxValue;
        backgroundColor   = D2D1::ColorF(0.2f, 0.2f, 0.2f, 1.f);
        fillColor         = D2D1::ColorF(0.2f, 0.8f, 0.2f, 1.f);
        borderColor       = D2D1::ColorF(0.1f, 0.1f, 0.1f, 1.f);
    }

    Object* Slider::Clone()
    {
        auto* clone = new Slider();
        InitClone(clone);
        clone->minValue         = minValue;
        clone->maxValue         = maxValue;
        clone->m_value          = m_value;
        clone->width            = width;
        clone->height           = height;
        clone->direction        = direction;
        clone->backgroundImage  = backgroundImage;
        clone->fillImage        = fillImage;
        clone->backgroundColor  = backgroundColor;
        clone->fillColor        = fillColor;
        clone->borderColor      = borderColor;
        clone->onValueChanged   = onValueChanged;
        return clone;
    }

    // ---------------------------------------------------------------------------
    // Render
    // ---------------------------------------------------------------------------

    void Slider::Render(ID2D1DeviceContext7* ctx)
    {
        EnsureResources(ctx);

        D2D1::Matrix3x2F screen;
        ctx->GetTransform(&screen);

        if (backgroundImage.GetBitmap() || fillImage.GetBitmap())
            RenderSpriteBar(ctx, screen);
        else
            RenderSolidBar(ctx, screen);

        ctx->SetTransform(screen);
    }

    // ---------------------------------------------------------------------------
    // Solid color bar
    // ---------------------------------------------------------------------------

    void Slider::RenderSolidBar(ID2D1DeviceContext7* ctx, const D2D1::Matrix3x2F& screen)
    {
        Transform* tf = GetOwner()->GetTransform();
        Vector2    wp = tf->GetWorldPosition();

        float hw = width  * 0.5f;
        float hh = height * 0.5f;

        // background
        D2D1_RECT_F bg = { wp.x - hw, wp.y - hh, wp.x + hw, wp.y + hh };
        m_brush->SetColor(backgroundColor);
        ctx->FillRectangle(bg, m_brush.Get());

        // fill
        float t = GetNormalizedValue();
        D2D1_RECT_F fill = bg;

        switch (direction)
        {
        case Direction::LeftToRight:
            fill.right  = fill.left + width * t;
            break;
        case Direction::RightToLeft:
            fill.left   = fill.right - width * t;
            break;
        case Direction::BottomToTop:
            fill.top    = fill.bottom - height * t;
            break;
        case Direction::TopToBottom:
            fill.bottom = fill.top + height * t;
            break;
        }

        m_brush->SetColor(fillColor);
        ctx->FillRectangle(fill, m_brush.Get());

        // border
        m_brush->SetColor(borderColor);
        ctx->DrawRectangle(bg, m_brush.Get(), 1.f);
    }

    // ---------------------------------------------------------------------------
    // Sprite bar
    // ---------------------------------------------------------------------------

    void Slider::RenderSpriteBar(ID2D1DeviceContext7* ctx, const D2D1::Matrix3x2F& screen)
    {
        Transform* tf   = GetOwner()->GetTransform();
        const auto& world = tf->GetWorldMatrix();

        D2D1::Matrix3x2F d2dMatrix(
            world._11, world._12,
            world._21, world._22,
            world._41, world._42
        );

        float t = GetNormalizedValue();

        // background sprite (full size)
        if (backgroundImage.GetBitmap())
        {
            float bw = (float)backgroundImage.GetWidth();
            float bh = (float)backgroundImage.GetHeight();
            float pw = bw * backgroundImage.pivot.x;
            float ph = bh * backgroundImage.pivot.y;

            D2D1::Matrix3x2F mat =
                D2D1::Matrix3x2F::Translation(-pw, -ph) * d2dMatrix * screen;
            DrawSprite(ctx, backgroundImage, mat, bw, bh);
        }

        // fill sprite: clip srcRect to normalized value along the fill direction
        if (fillImage.GetBitmap())
        {
            float fw = (float)fillImage.GetWidth();
            float fh = (float)fillImage.GetHeight();

            // Determine the visible portion of the fill sprite
            D2D1_RECT_F src;
            float dstW = fw, dstH = fh;
            float pivotOffX = 0.f, pivotOffY = 0.f;

            switch (direction)
            {
            case Direction::LeftToRight:
                src    = { 0.f, 0.f, fw * t, fh };
                dstW   = fw * t;
                pivotOffX = fw * fillImage.pivot.x;
                pivotOffY = fh * fillImage.pivot.y;
                break;
            case Direction::RightToLeft:
                src    = { fw * (1.f - t), 0.f, fw, fh };
                dstW   = fw * t;
                pivotOffX = fw * fillImage.pivot.x - fw * (1.f - t);
                pivotOffY = fh * fillImage.pivot.y;
                break;
            case Direction::BottomToTop:
                src    = { 0.f, fh * (1.f - t), fw, fh };
                dstH   = fh * t;
                pivotOffX = fw * fillImage.pivot.x;
                pivotOffY = fh * fillImage.pivot.y - fh * (1.f - t);
                break;
            case Direction::TopToBottom:
                src    = { 0.f, 0.f, fw, fh * t };
                dstH   = fh * t;
                pivotOffX = fw * fillImage.pivot.x;
                pivotOffY = fh * fillImage.pivot.y;
                break;
            }

            fillImage.srcRect = src;

            D2D1::Matrix3x2F mat =
                D2D1::Matrix3x2F::Translation(-pivotOffX, -pivotOffY) * d2dMatrix * screen;
            DrawSprite(ctx, fillImage, mat, dstW, dstH);

            fillImage.srcRect = std::nullopt;
        }
    }

    // ---------------------------------------------------------------------------
    // Draw a single sprite with tint (mirrors SpriteRenderer logic)
    // ---------------------------------------------------------------------------

    void Slider::DrawSprite(ID2D1DeviceContext7* ctx, Sprite& spr,
                            const D2D1::Matrix3x2F& transform, float dstW, float dstH)
    {
        ctx->SetTransform(transform);

        const D2D1_COLOR_F& t = spr.tint;
        const bool needsTint  = (t.r != 1.f || t.g != 1.f || t.b != 1.f || t.a != 1.f);

        D2D1_RECT_F dstRect = { 0.f, 0.f, dstW, dstH };

        if (needsTint)
        {
            if (!m_colorEffect)
                ctx->CreateEffect(CLSID_D2D1ColorMatrix, &m_colorEffect);

            D2D1_MATRIX_5X4_F mat = {};
            mat.m[0][0] = t.r; mat.m[1][1] = t.g;
            mat.m[2][2] = t.b; mat.m[3][3] = t.a;
            m_colorEffect->SetValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX, mat);

            Microsoft::WRL::ComPtr<ID2D1Image> bitmapImage;
            spr.GetBitmap()->QueryInterface(IID_PPV_ARGS(&bitmapImage));
            m_colorEffect->SetInput(0, bitmapImage.Get());

            Microsoft::WRL::ComPtr<ID2D1Image> effectImage;
            m_colorEffect.As(&effectImage);

            if (spr.srcRect)
            {
                D2D1_RECT_F srcRectF = spr.srcRect.value();
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
            if (spr.srcRect)
            {
                D2D1_RECT_F srcRectF = spr.srcRect.value();
                ctx->DrawBitmap(spr.GetBitmap(), &dstRect, t.a,
                    D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &srcRectF);
            }
            else
            {
                ctx->DrawBitmap(spr.GetBitmap(), &dstRect, t.a,
                    D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            }
        }
    }

    void Slider::EnsureResources(ID2D1DeviceContext7* ctx)
    {
        if (!m_brush)
            ctx->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_brush);
    }
}
