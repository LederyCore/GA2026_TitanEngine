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

        // -----------------------------------------------------------------------
        // RectLayout
        // The TitanEngine equivalent of giving Unity's Background image and Fill
        // image each their own child RectTransform (position / size / pivot).
        // Every field is a local override; leaving it at its default makes the
        // graphic inherit the slider's shared box, so old setups keep working.
        // -----------------------------------------------------------------------
        struct RectLayout
        {
            // Extra pixel offset for THIS graphic, added on top of Slider::offset.
            // (Like nudging the child RectTransform's anchored position.)
            D2D1_POINT_2F offset = { 0.f, 0.f };

            // Box size in pixels. <= 0 means "inherit the slider's width / height".
            float width  = -1.f;
            float height = -1.f;

            // Pivot/anchor inside this box, normalized 0~1.
            // x < 0 means "inherit the slider's pivot".
            // {0,0}=top-left, {0.5,0.5}=center, {1,1}=bottom-right.
            D2D1_POINT_2F pivot = { -1.f, -1.f };
        };

        // --- Value range ---
        float minValue = 0.f;
        float maxValue = 1.f;

        // --- Shared box (inherited by any graphic that does not override it) ---
        float width  = 200.f;
        float height = 20.f;
        D2D1_POINT_2F pivot  = { 0.5f, 0.5f };  // shared pivot
        D2D1_POINT_2F offset = { 0.f, 0.f };    // shared offset

        // --- Independent layout per graphic (set these to place them freely) ---
        RectLayout backgroundRect;   // background image / color bar
        RectLayout fillRect;         // fill track; visible fill grows inside this box

        Direction direction = Direction::LeftToRight;

        // --- Sprite mode: set a texture to render an image instead of a solid color ---
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

        // Resolve only the box size of a layout (applies the shared-size fallback).
        void ResolveSize(const RectLayout& layout, float& outW, float& outH) const;

        // Resolve a layout into size / pivot / offset against the shared defaults.
        void ResolveLayout(const RectLayout& layout,
                           float& outW, float& outH,
                           D2D1_POINT_2F& outPivot,
                           D2D1_POINT_2F& outOffset) const;

        // Box-space transform for a graphic, anchored by pivot/offset (box = [0,0]~[w,h]).
        D2D1::Matrix3x2F BuildBoxMatrix(const RectLayout& layout,
                                        const D2D1::Matrix3x2F& screen);

        // Sub-rect of a (w x h) box that the fill covers for value t / direction.
        D2D1_RECT_F ComputeFillRect(float w, float h, float t) const;

        // Draw a sprite stretched into dstRect (optionally clipping srcRect), with tint.
        void DrawSprite(ID2D1DeviceContext7* ctx, Sprite& spr,
                        const D2D1::Matrix3x2F& box,
                        const D2D1_RECT_F& dstRect,
                        const std::optional<D2D1_RECT_F>& srcRect);
    };
}
