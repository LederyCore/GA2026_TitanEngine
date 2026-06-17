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

        // --- Layout (Unity RectTransform style) ---
        // Pivot/anchor of the slider box (width x height), normalized 0~1.
        // {0,0}=top-left, {0.5,0.5}=center, {1,1}=bottom-right.
        // Both the color bar and the background image are laid out from this anchor.
        D2D1_POINT_2F pivot = { 0.5f, 0.5f };

        // Extra rendering start offset (in pixels) applied on top of the pivot,
        // so the draw origin can be nudged freely like Unity.
        D2D1_POINT_2F offset = { 0.f, 0.f };

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

        // Local box-space transform anchored by pivot/offset (box = [0,0]~[width,height]).
        D2D1::Matrix3x2F BuildBoxMatrix(const D2D1::Matrix3x2F& screen);

        // Sub-rect of the box that the fill covers for the current value/direction.
        D2D1_RECT_F      ComputeFillRect(float t) const;

        // Draw a sprite stretched into dstRect (optionally clipping a srcRect), with tint.
        void DrawSprite(ID2D1DeviceContext7* ctx, Sprite& spr,
                        const D2D1::Matrix3x2F& box,
                        const D2D1_RECT_F& dstRect,
                        const std::optional<D2D1_RECT_F>& srcRect);
    };
}
