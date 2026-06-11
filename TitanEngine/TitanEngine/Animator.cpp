#include "pch.h"
#include "Animator.h"
#include "AnimationClip.h"
#include "SpriteRenderer.h"
#include "GameObject.h"

namespace TitanEngine
{
    // ── 클립 등록 ─────────────────────────────────────────────────────

    void Animator::AddClip(std::shared_ptr<AnimationClip> clip)
    {
        if (clip) m_clips[clip->name] = clip;
    }

    void Animator::AddClip(const std::string& name, std::shared_ptr<AnimationClip> clip)
    {
        if (clip) m_clips[name] = clip;
    }

    // ── 재생 제어 ──────────────────────────────────────────────────────

    void Animator::Play(const std::string& clipName, float normalizedTime)
    {
        auto it = m_clips.find(clipName);
        if (it == m_clips.end()) return;

        m_currentClipName = clipName;
        m_playing         = true;
        m_elapsed         = normalizedTime * it->second->GetTotalDuration();

        ApplyFrame();
    }

    bool Animator::IsPlaying(const std::string& clipName) const
    {
        return m_playing && m_currentClipName == clipName;
    }

    float Animator::GetNormalizedTime() const
    {
        auto it = m_clips.find(m_currentClipName);
        if (it == m_clips.end() || it->second->GetTotalDuration() == 0.f)
            return 0.f;
        return m_elapsed / it->second->GetTotalDuration();
    }

    // ── IUpdateable::Update ────────────────────────────────────────────

    void Animator::Update(float deltaTime)
    {
        if (!m_playing) return;

        auto it = m_clips.find(m_currentClipName);
        if (it == m_clips.end()) return;

        AnimationClip* clip = it->second.get();
        if (clip->GetFrames().empty()) return;

        m_elapsed += deltaTime * m_speed;

        float total = clip->GetTotalDuration();
        if (clip->loop)
        {
            if (m_elapsed >= total)
                m_elapsed = std::fmod(m_elapsed, total);
        }
        else
        {
            if (m_elapsed >= total)
            {
                m_elapsed = total;
                m_playing = false;
            }
        }

        ApplyFrame();

        // 트리거는 한 프레임 소비 후 자동 리셋
        for (auto& [name, val] : m_triggers)
            val = false;
    }

    // ── 내부 헬퍼 ─────────────────────────────────────────────────────

    void Animator::ApplyFrame()
    {
        auto it = m_clips.find(m_currentClipName);
        if (it == m_clips.end()) return;

        AnimationClip* clip = it->second.get();
        const auto& frames  = clip->GetFrames();
        if (frames.empty()) return;

        // elapsed에 해당하는 프레임 탐색
        float accum = 0.f;
        const AnimFrame* cur = &frames.back();
        for (const auto& f : frames)
        {
            accum += f.duration;
            if (m_elapsed < accum) { cur = &f; break; }
        }

        SpriteRenderer* sr = GetRenderer();
        if (!sr) return;

        sr->sprite.texture = clip->GetTexture();
        sr->sprite.srcRect = cur->ToRectF();
    }

    SpriteRenderer* Animator::GetRenderer()
    {
        return GetOwner()->GetComponent<SpriteRenderer>();
    }

    // ── Clone ──────────────────────────────────────────────────────────

    Object* Animator::Clone()
    {
        auto* clone = new Animator();
        InitClone(clone);
        clone->m_clips           = m_clips;
        clone->m_currentClipName = m_currentClipName;
        clone->m_elapsed         = m_elapsed;
        clone->m_speed           = m_speed;
        clone->m_playing         = m_playing;
        clone->m_bools           = m_bools;
        clone->m_floats          = m_floats;
        clone->m_ints            = m_ints;
        clone->m_triggers        = m_triggers;
        return clone;
    }

    // ── 파라미터 ──────────────────────────────────────────────────────

    void  Animator::SetBool    (const std::string& name, bool  v) { m_bools[name]   = v; }
    void  Animator::SetFloat   (const std::string& name, float v) { m_floats[name]  = v; }
    void  Animator::SetInt     (const std::string& name, int   v) { m_ints[name]    = v; }
    void  Animator::SetTrigger (const std::string& name)          { m_triggers[name] = true; }
    void  Animator::ResetTrigger(const std::string& name)         { m_triggers[name] = false; }

    bool  Animator::GetBool (const std::string& name) const
    { auto it = m_bools.find(name);  return it != m_bools.end()  ? it->second : false; }
    float Animator::GetFloat(const std::string& name) const
    { auto it = m_floats.find(name); return it != m_floats.end() ? it->second : 0.f;   }
    int   Animator::GetInt  (const std::string& name) const
    { auto it = m_ints.find(name);   return it != m_ints.end()   ? it->second : 0;     }
}
