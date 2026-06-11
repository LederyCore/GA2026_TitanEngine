#pragma once
#include "Component.h"
#include "Sprite.h"
#include <d2d1effects.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace TitanEngine
{
    class SpriteRenderer : public Component, public IRenderable
    {
    public:
        Sprite sprite;

        Object* Clone() override;
        void Render(ID2D1DeviceContext7* ctx) override;

    private:
        // 색조 적용용 ColorMatrix Effect. 컨텍스트당 한 번만 생성.
        ComPtr<ID2D1Effect> m_colorEffect;

        void EnsureEffect(ID2D1DeviceContext7* ctx);
    };
}
