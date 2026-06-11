#pragma once
#include "Component.h"
#include <functional>
#include <string>
#include <wrl/client.h>

namespace TitanEngine
{
    class SpriteRenderer;

    class Button : public Component, public IUpdateable, public IRenderable
    {
    public:
        struct ColorBlock
        {
            D2D1_COLOR_F normalColor;
            D2D1_COLOR_F highlightedColor;
            D2D1_COLOR_F pressedColor;
            D2D1_COLOR_F disabledColor;
            float        fadeDuration    = 0.1f;
            float        colorMultiplier = 1.f;

            ColorBlock();
        };

        std::function<void()> onClick;

        bool       interactable = true;
        ColorBlock colors;

        float width  = 160.f;
        float height = 30.f;

        std::wstring text;
        D2D1_COLOR_F textColor;
        float        fontSize = 14.f;

        SpriteRenderer* targetGraphic = nullptr;

        Object* Clone() override;
        void OnAwake()  override;
        void Update(float deltaTime) override;
        void Render(ID2D1DeviceContext7* ctx) override;

        static void SetScreenSize(float w, float h) { s_screenW = w; s_screenH = h; }

    private:
        enum class SelectionState { Normal, Highlighted, Pressed, Disabled };

        SelectionState m_state      = SelectionState::Normal;
        D2D1_COLOR_F   m_currentTint;
        D2D1_COLOR_F   m_fromTint;
        float          m_fadeTimer  = 0.f;
        bool           m_wasPressed = false;

        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_brush;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_textBrush;
        Microsoft::WRL::ComPtr<IDWriteFactory>        m_dwriteFactory;
        Microsoft::WRL::ComPtr<IDWriteTextFormat>     m_textFormat;

        bool         IsPointInBounds(float px, float py);
        D2D1_COLOR_F GetTargetColor() const;
        void         StartTransition(SelectionState newState);
        void         ApplyTint(const D2D1_COLOR_F& tint);
        void         EnsureResources(ID2D1DeviceContext7* ctx);

        static D2D1_COLOR_F LerpColor(const D2D1_COLOR_F& a, const D2D1_COLOR_F& b, float t);
        static D2D1_COLOR_F MultiplyColor(const D2D1_COLOR_F& c, float mult);

        static float s_screenW;
        static float s_screenH;
    };
}
