#include "pch.h"
#include "AnimationClip.h"

namespace TitanEngine
{
    void AnimationClip::AddFrames(UINT frameW, UINT frameH, UINT frameCount, float totalDuration)
    {
        if (frameCount == 0 || totalDuration <= 0.f) return;

        const float perFrame = totalDuration / static_cast<float>(frameCount);

        for (UINT i = 0; i < frameCount; ++i)
        {
            AnimFrame f;
            f.srcRect  = { i * frameW, 0, (i + 1) * frameW, frameH };
            f.duration = perFrame;
            m_frames.push_back(f);
        }

        m_totalDuration += totalDuration;
    }
}
