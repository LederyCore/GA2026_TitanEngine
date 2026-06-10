#pragma once
#include <vector>
#include <memory>
#include <string>
#include <d2d1_3.h>

namespace TitanEngine
{
    class Texture2D;

    struct AnimFrame
    {
        D2D1_RECT_U srcRect  = {};
        float       duration = 0.f;

        float Width()  const { return static_cast<float>(srcRect.right  - srcRect.left); }
        float Height() const { return static_cast<float>(srcRect.bottom - srcRect.top);  }

        D2D1_RECT_F ToRectF() const
        {
            return {
                static_cast<float>(srcRect.left),
                static_cast<float>(srcRect.top),
                static_cast<float>(srcRect.right),
                static_cast<float>(srcRect.bottom)
            };
        }
    };

    class AnimationClip
    {
    public:
        std::string name;
        bool        loop = true;

        void SetTexture(std::shared_ptr<Texture2D> tex) { m_texture = std::move(tex); }

        // 가로로 나열된 스프라이트 시트에서 프레임을 자동 분할하여 추가.
        // frameW/H : 단일 프레임 크기(px), frameCount : 총 프레임 수, totalDuration : 전체 재생 시간(초)
        void AddFrames(UINT frameW, UINT frameH, UINT frameCount, float totalDuration);

        const std::vector<AnimFrame>& GetFrames()        const { return m_frames; }
        float                         GetTotalDuration() const { return m_totalDuration; }
        std::shared_ptr<Texture2D>    GetTexture()       const { return m_texture; }

    private:
        std::shared_ptr<Texture2D> m_texture;
        std::vector<AnimFrame>     m_frames;
        float                      m_totalDuration = 0.f;
    };
}
