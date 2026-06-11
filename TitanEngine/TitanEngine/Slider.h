#pragma once
#include "Component.h"
#include "Sprite.h"
#include <functional>
#include <wrl/client.h>

namespace TitanEngine
{
    class Slider : public Component, public IRenderable
    {
    public:
        enum class Direction { LeftToRight, RightToLeft, BottomToTop, TopToBottom };

        // --- Value ---
        float minValue = 0.f;
        float maxValue = 1.f;

        // --- Size ---
        float width  = 200.f;
        float height = 20.f;

        Direction direction = Direction::LeftToRight;

        // --- Sprite mode: set texture to use images instead of solid colors ---
        Sprite backgroundImage;
        Sprite fillImage;

        // --- Solid color mode (used when no sprite is set) ---
        D2D1_COLOR_F backgroundColor;
        D2D1_COLOR_F fillColor;
        D2D1_COLOR_F borderColor;

        // --- Callback (Unity Slider.onValueChanged equivalent) ---
        std::function<void(float)> onValueChanged;

        // --- API ---
        void  SetValue(float v);
        float GetValue()           const { return m_value; }
        float GetNormalizedValue() const;

        Object* Clone() override;
        void OnAwake() override;
        void Render(ID2D1DeviceContext7* ctx) override;

    private:
        float m_value = 1.f;

        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_brush;
        Microsoft::WRL::ComPtr<ID2D1Effect>           m_colorEffect;

        void EnsureResources(ID2D1DeviceContext7* ctx);
        void RenderSolidBar(ID2D1DeviceContext7* ctx, const D2D1::Matrix3x2F& screen);
        void RenderSpriteBar(ID2D1DeviceContext7* ctx, const D2D1::Matrix3x2F& screen);
        void DrawSprite(ID2D1DeviceContext7* ctx, Sprite& spr,
                        const D2D1::Matrix3x2F& transform, float dstW, float dstH);
    };
}
