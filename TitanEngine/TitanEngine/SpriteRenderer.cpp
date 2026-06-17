#include "pch.h"
#include "SpriteRenderer.h"
#include "GameObject.h"
#include "Transform.h"
#include <d2d1effects.h>
#pragma comment(lib, "dxguid.lib")

namespace TitanEngine
{
    void SpriteRenderer::EnsureEffect(ID2D1DeviceContext7* ctx)
    {
        if (!m_colorEffect)
            ctx->CreateEffect(CLSID_D2D1ColorMatrix, &m_colorEffect);
    }

    void SpriteRenderer::Render(ID2D1DeviceContext7* ctx)
    {
        if (!sprite.GetBitmap()) return;

        Transform* tf = GetOwner()->GetTransform();
        const auto& world = tf->GetWorldMatrix();

        // SimpleMath Matrix (row-major 4x4) -> D2D Matrix3x2F
        D2D1::Matrix3x2F d2dMatrix(
            world._11, world._12,
            world._21, world._22,
            world._41, world._42
        );

        float drawW = (float)sprite.GetWidth();
        float drawH = (float)sprite.GetHeight();
        float pw = drawW * sprite.pivot.x;
        float ph = drawH * sprite.pivot.y;

        // Scene이 설정한 screenTransform(화면 중앙 오프셋)을 유지하기 위해 현재 transform을 읽어서 뒤에 곱함
        D2D1::Matrix3x2F screenTransform;
        ctx->GetTransform(&screenTransform);

        const D2D1_COLOR_F& t = sprite.tint;
        const bool needsTint = (t.r != 1.f || t.g != 1.f || t.b != 1.f);
        const bool hasSrc = sprite.srcRect.has_value();

        if (needsTint)
        {
            D2D1::Matrix3x2F finalMatrix =
                D2D1::Matrix3x2F::Translation(-pw, -ph) * d2dMatrix * screenTransform;
            ctx->SetTransform(finalMatrix);

            EnsureEffect(ctx);

            // Scale each RGBA channel by tint values
            D2D1_MATRIX_5X4_F mat = {};
            mat.m[0][0] = t.r;
            mat.m[1][1] = t.g;
            mat.m[2][2] = t.b;
            mat.m[3][3] = t.a;
            m_colorEffect->SetValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX, mat);

            ComPtr<ID2D1Image> bitmapImage;
            sprite.GetBitmap()->QueryInterface(IID_PPV_ARGS(&bitmapImage));
            m_colorEffect->SetInput(0, bitmapImage.Get());

            // imageRectangle으로 srcRect 영역만 렌더링
            // ComPtr::As 로 ID2D1Image* 획득 후 5-param 오버로드 사용
            ComPtr<ID2D1Image> effectImage;
            m_colorEffect.As(&effectImage);
            if (hasSrc)
            {
                D2D1_RECT_F srcRectF = sprite.srcRect.value();
                ctx->DrawImage(effectImage.Get(), (const D2D1_POINT_2F*)nullptr, &srcRectF,
                    D2D1_INTERPOLATION_MODE_LINEAR, D2D1_COMPOSITE_MODE_SOURCE_OVER);
            }
            else
            {
                ctx->DrawImage(effectImage.Get(), (const D2D1_POINT_2F*)nullptr, (const D2D1_RECT_F*)nullptr,
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
                D2D1_RECT_F srcRectF = sprite.srcRect.value();
                ctx->DrawBitmap(sprite.GetBitmap(), &dstRect, t.a,
                    D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &srcRectF);
            }
            else
            {
                ctx->DrawBitmap(sprite.GetBitmap(), &dstRect, t.a,
                    D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            }
        }

        ctx->SetTransform(screenTransform);
    }

    Object* SpriteRenderer::Clone()
    {
        auto* clone = new SpriteRenderer();
        InitClone(clone);
        clone->sprite = sprite;
        return clone;
    }
}
