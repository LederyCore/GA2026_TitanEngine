#include "pch.h"
#include "AudioComponent.h"
#include "AudioManager.h"

namespace TitanEngine
{
    AudioComponent::~AudioComponent()
    {
        ReleaseVoice();
    }

    Object* AudioComponent::Clone()
    {
        auto* clone = new AudioComponent();
        InitClone(clone);
        clone->m_clip        = m_clip;          // 클립은 공유(shared_ptr)
        clone->m_volume      = m_volume;
        clone->m_pitch       = m_pitch;
        clone->m_loop        = m_loop;
        clone->m_playOnAwake = m_playOnAwake;
        clone->m_category    = m_category;
        // 보이스는 복제하지 않는다. 재생 시 각자 새로 생성.
        return clone;
    }

    void AudioComponent::OnStart()
    {
        if (m_playOnAwake)
            Play();
    }

    void AudioComponent::OnDisable()
    {
        Stop();
    }

    void AudioComponent::OnDestory()
    {
        ReleaseVoice();
    }

    void AudioComponent::SetClip(const std::shared_ptr<AudioClip>& clip)
    {
        if (m_clip == clip)
            return;

        // 클립이 바뀌면 기존 포맷의 보이스는 더 이상 유효하지 않다.
        ReleaseVoice();
        m_clip = clip;
    }

    void AudioComponent::EnsureVoice()
    {
        if (m_voice || !m_clip || !m_clip->IsValid())
            return;
        m_voice = AudioManager::Instance().CreateSourceVoice(m_clip->GetFormat());
    }

    void AudioComponent::ReleaseVoice()
    {
        if (!m_voice)
            return;
        m_voice->Stop(0);
        m_voice->FlushSourceBuffers();
        m_voice->DestroyVoice();
        m_voice = nullptr;
    }

    void AudioComponent::Play()
    {
        if (!m_clip || !m_clip->IsValid())
            return;

        EnsureVoice();
        if (!m_voice)
            return;

        // 처음부터 다시 재생: 정지 후 버퍼 비우고 재제출
        m_voice->Stop(0);
        m_voice->FlushSourceBuffers();

        XAUDIO2_BUFFER buffer{};
        buffer.AudioBytes = m_clip->GetSize();
        buffer.pAudioData = m_clip->GetData();
        buffer.Flags      = XAUDIO2_END_OF_STREAM;
        buffer.LoopCount  = m_loop ? XAUDIO2_LOOP_INFINITE : 0;

        if (FAILED(m_voice->SubmitSourceBuffer(&buffer)))
            return;

        const float catVol = AudioManager::Instance().GetCategoryVolume(m_category);
        m_voice->SetVolume(m_volume * catVol);
        m_voice->SetFrequencyRatio(m_pitch);
        m_voice->Start(0);
        m_paused = false;
    }

    void AudioComponent::Stop()
    {
        if (!m_voice)
            return;
        m_voice->Stop(0);
        m_voice->FlushSourceBuffers();
        m_paused = false;
    }

    void AudioComponent::Pause()
    {
        if (m_voice)
        {
            m_voice->Stop(0);
            m_paused = true;
        }
    }

    void AudioComponent::Resume()
    {
        if (m_voice && m_paused)
        {
            m_voice->Start(0);
            m_paused = false;
        }
    }

    bool AudioComponent::IsPlaying() const
    {
        if (!m_voice || m_paused)
            return false;
        XAUDIO2_VOICE_STATE state{};
        m_voice->GetState(&state, XAUDIO2_VOICE_NOSAMPLESPLAYED);
        return state.BuffersQueued > 0;
    }

    void AudioComponent::SetVolume(float volume)
    {
        m_volume = volume;
        if (m_voice)
        {
            const float catVol = AudioManager::Instance().GetCategoryVolume(m_category);
            m_voice->SetVolume(m_volume * catVol);
        }
    }

    void AudioComponent::SetPitch(float ratio)
    {
        m_pitch = ratio;
        if (m_voice)
            m_voice->SetFrequencyRatio(m_pitch);
    }
}
