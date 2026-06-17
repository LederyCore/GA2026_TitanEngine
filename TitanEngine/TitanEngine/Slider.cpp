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
        clone->pivot            = pivot;
        clone->offset           = offset;
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
    //
    // The slider is a single box of (width x height) anchored by pivot/offset.
    // Background and fill are drawn as two independent layers; each layer uses its
    // sprite when one is assigned, otherwise falls back to its solid color. This
    // means assigning only a background image still shows the default color bar.
    // ---------------------------------------------------------------------------

    void Slider::Render(ID2D1DeviceContext7* ctx)
    {
        EnsureResources(ctx);

        D2D1::Matrix3x2F screen;
        ctx->GetTransform(&screen);

        const D2D1::Matrix3x2F box = BuildBoxMatrix(screen);
        const D2D1_RECT_F boxRect  = { 0.f, 0.f, width, height };

        // --- Background layer ---
        if (backgroundImage.GetBitmap())
        {
            DrawSprite(ctx, backgroundImage, box, boxRect, std::nullopt);
        }
        else
        {
            ctx->SetTransform(box);
            m_brush->SetColor(backgroundColor);
            ctx->FillRectangle(boxRect, m_brush.Get());
        }

        // --- Fill layer ---
        const float t = GetNormalizedValue();
        if (t > 0.f)
        {
            const D2D1_RECT_F dst = ComputeFillRect(t);

            if (fillImage.GetBitmap())
            {
                // Clip the matching portion of the fill sprite so it reveals/hides
                // from the correct edge as the value changes.
                const float fw = (float)fillImage.GetWidth();
                const float fh = (float)fillImage.GetHeight();
                D2D1_RECT_F src;
                switch (direction)
                {
                case Direction::LeftToRight: src = { 0.f, 0.f, fw * t, fh }; break;
                case Direction::RightToLeft: src = { fw * (1.f - t), 0.f, fw, fh }; break;
                case Direction::BottomToTop: src = { 0.f, fh * (1.f - t), fw, fh }; break;
                case Direction::TopToBottom: src = { 0.f, 0.f, fw, fh * t }; break;
                }
                DrawSprite(ctx, fillImage, box, dst, src);
            }
            else
            {
                ctx->SetTransform(box);
                m_brush->SetColor(fillColor);
                ctx->FillRectangle(dst, m_brush.Get());
            }
        }

        // --- Border (only meaningful for the solid look) ---
        if (!backgroundImage.GetBitmap())
        {
            ctx->SetTransform(box);
            m_brush->SetColor(borderColor);
            ctx->DrawRectangle(boxRect, m_brush.Get(), 1.f);
        }

        ctx->SetTransform(screen);
    }

    // ---------------------------------------------------------------------------
    // Layout helpers
    // ---------------------------------------------------------------------------

    D2D1::Matrix3x2F Slider::BuildBoxMatrix(const D2D1::Matrix3x2F& screen)
    {
        Transform* tf     = GetOwner()->GetTransform();
        const auto& world = tf->GetWorldMatrix();

        // Full world matrix so the slider honors the transform's scale/rotation.
        D2D1::Matrix3x2F d2dMatrix(
            world._11, world._12,
            world._21, world._22,
            world._41, world._42
        );

        // Move the box so the pivot lands on the transform origin, then nudge by offset.
        const float ox = -pivot.x * width  + offset.x;
        const float oy = -pivot.y * height + offset.y;

        return D2D1::Matrix3x2F::Translation(ox, oy) * d2dMatrix * screen;
    }

    D2D1_RECT_F Slider::ComputeFillRect(float t) const
    {
        D2D1_RECT_F r = { 0.f, 0.f, width, height };
        switch (direction)
        {
        case Direction::LeftToRight: r.right  = width  * t;            break;
        case Direction::RightToLeft: r.left   = width  * (1.f - t);    break;
        case Direction::BottomToTop: r.top    = height * (1.f - t);    break;
        case Direction::TopToBottom: r.bottom = height * t;            break;
        }
        return r;
    }

    // ---------------------------------------------------------------------------
    // Draw a sprite stretched into dstRect (box space), optionally clipping srcRect.
    // Mirrors SpriteRenderer's tint handling.
    // ---------------------------------------------------------------------------

    void Slider::DrawSprite(ID2D1DeviceContext7* ctx, Sprite& spr,
                            const D2D1::Matrix3x2F& box,
                            const D2D1_RECT_F& dstRect,
                            const std::optional<D2D1_RECT_F>& srcRect)
    {
        const D2D1_COLOR_F& t = spr.tint;
        const bool needsTint  = (t.r != 1.f || t.g != 1.f || t.b != 1.f || t.a != 1.f);

        const float dstW = dstRect.right  - dstRect.left;
        const float dstH = dstRect.bottom - dstRect.top;
        if (dstW <= 0.f || dstH <= 0.f) return;

        if (needsTint)
        {
            // DrawImage doesn't stretch on its own, so bake the src->dst scale into
            // the transform (scaled about the dst origin) and draw the src region there.
            const float srcW = srcRect ? (srcRect->right  - srcRect->left) : (float)spr.GetWidth();
            const float srcH = srcRect ? (srcRect->bottom - srcRect->top)  : (float)spr.GetHeight();
            const float sx = (srcW > 0.f) ? dstW / srcW : 1.f;
            const float sy = (srcH > 0.f) ? dstH / srcH : 1.f;

            const D2D1_POINT_2F origin = { dstRect.left, dstRect.top };
            ctx->SetTransform(D2D1::Matrix3x2F::Scale(sx, sy, origin) * box);

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

            if (srcRect)
            {
                D2D1_RECT_F srcF = srcRect.value();
                ctx->DrawImage(effectImage.Get(), &origin, &srcF,
                    D2D1_INTERPOLATION_MODE_LINEAR, D2D1_COMPOSITE_MODE_SOURCE_OVER);
            }
            else
            {
                ctx->DrawImage(effectImage.Get(), &origin, nullptr,
                    D2D1_INTERPOLATION_MODE_LINEAR, D2D1_COMPOSITE_MODE_SOURCE_OVER);
            }
        }
        else
        {
            ctx->SetTransform(box);
            if (srcRect)
            {
                D2D1_RECT_F srcF = srcRect.value();
                ctx->DrawBitmap(spr.GetBitmap(), &dstRect, t.a,
                    D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &srcF);
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
