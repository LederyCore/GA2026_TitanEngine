#pragma once
#include "Component.h"
#include "AudioClip.h"
#include "AudioManager.h"   // AudioCategory
#include <xaudio2.h>
#include <memory>

namespace TitanEngine
{
    // GameObject 에 부착되어 하나의 AudioClip 을 재생/제어하는 컴포넌트.
    // BGM 처럼 지속 재생되는 사운드나 특정 오브젝트의 효과음에 사용한다.
    // 실제 재생 엔진(XAudio2)은 AudioManager 가 제공하는 SourceVoice 를 사용한다.
    class AudioComponent : public Component
    {
    public:
        ~AudioComponent() override;

        Object* Clone() override;

        void OnStart() override;    // PlayOnAwake 처리
        void OnDisable() override;  // 비활성화 시 정지
        void OnDestory() override;  // 보이스 해제

        // 재생할 클립 설정. ResourceManager::Load<AudioClip>(path) 로 얻은 것을 넘긴다.
        void SetClip(const std::shared_ptr<AudioClip>& clip);
        const std::shared_ptr<AudioClip>& GetClip() const { return m_clip; }

        void Play();    // 처음부터 재생
        void Stop();
        void Pause();
        void Resume();

        bool IsPlaying() const;

        void SetLoop(bool loop)       { m_loop = loop; }
        bool IsLoop() const           { return m_loop; }

        void SetPlayOnAwake(bool v)   { m_playOnAwake = v; }
        bool IsPlayOnAwake() const    { return m_playOnAwake; }

        // 볼륨 (0.0 ~ 1.0 이상, 1.0 = 원음)
        void  SetVolume(float volume);
        float GetVolume() const { return m_volume; }

        // 재생 속도/피치 (1.0 = 원속, 0.5~2.0 권장)
        void  SetPitch(float ratio);
        float GetPitch() const { return m_pitch; }

        // 사운드 분류 (볼륨 그룹)
        void          SetCategory(AudioCategory category) { m_category = category; }
        AudioCategory GetCategory() const { return m_category; }

    private:
        void EnsureVoice();   // 클립 포맷에 맞는 보이스 확보
        void ReleaseVoice();

    private:
        std::shared_ptr<AudioClip> m_clip;
        IXAudio2SourceVoice* m_voice = nullptr;

        float         m_volume   = 1.0f;
        float         m_pitch    = 1.0f;
        bool          m_loop     = false;
        bool          m_playOnAwake = false;
        bool          m_paused   = false;
        AudioCategory m_category = AudioCategory::SFX;
    };
}
