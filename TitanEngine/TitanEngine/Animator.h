#pragma once
#include "Component.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace TitanEngine
{
    class AnimationClip;
    class SpriteRenderer;

    class Animator : public Component, public IUpdateable
    {
    public:
        Object* Clone() override;
        void    Update(float deltaTime) override;

        void AddClip(std::shared_ptr<AnimationClip> clip);
        void AddClip(const std::string& name, std::shared_ptr<AnimationClip> clip);

        void Play(const std::string& clipName, float normalizedTime = 0.f);

        bool IsPlaying()                            const { return m_playing; }
        bool IsPlaying(const std::string& clipName) const;

        float GetSpeed()        const { return m_speed; }
        void  SetSpeed(float s)       { m_speed = s; }

        float GetNormalizedTime() const;
        const std::string& GetCurrentClipName() const { return m_currentClipName; }

        void  SetBool    (const std::string& name, bool  value);
        void  SetFloat   (const std::string& name, float value);
        void  SetInt     (const std::string& name, int   value);
        void  SetTrigger (const std::string& name);
        void  ResetTrigger(const std::string& name);

        bool  GetBool (const std::string& name) const;
        float GetFloat(const std::string& name) const;
        int   GetInt  (const std::string& name) const;

    private:
        void ApplyFrame();
        SpriteRenderer* GetRenderer();

    private:
        std::unordered_map<std::string, std::shared_ptr<AnimationClip>> m_clips;
        std::string m_currentClipName;
        float       m_elapsed = 0.f;
        float       m_speed   = 1.f;
        bool        m_playing = false;

        std::unordered_map<std::string, bool>  m_bools;
        std::unordered_map<std::string, float> m_floats;
        std::unordered_map<std::string, int>   m_ints;
        std::unordered_map<std::string, bool>  m_triggers;
    };
}
