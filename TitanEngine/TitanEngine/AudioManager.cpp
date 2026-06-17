#include "pch.h"
#include "AudioManager.h"
#include "AudioClip.h"
#include "DebugConsole/DebugConsole.h"

#include <mfapi.h>
#include <algorithm>

#pragma comment(lib, "xaudio2.lib")
#pragma comment(lib, "mfplat.lib")

namespace TitanEngine
{
    bool AudioManager::Initialize()
    {
        if (m_xaudio2)
            return true;   // 이미 초기화됨

        // Media Foundation 시작 (AudioClip 디코딩에 필요)
        HRESULT hr = MFStartup(MF_VERSION, MFSTARTUP_LITE);
        if (FAILED(hr))
        {
            LOG_ERROR("AudioManager - MFStartup 실패 (0x%08X)", hr);
            return false;
        }
        m_mfStarted = true;

        // XAudio2 엔진 생성
        hr = XAudio2Create(m_xaudio2.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR);
        if (FAILED(hr))
        {
            LOG_ERROR("AudioManager - XAudio2Create 실패 (0x%08X)", hr);
            MFShutdown();
            m_mfStarted = false;
            return false;
        }

        // 마스터링 보이스 생성 (최종 출력)
        hr = m_xaudio2->CreateMasteringVoice(&m_masteringVoice);
        if (FAILED(hr))
        {
            LOG_ERROR("AudioManager - CreateMasteringVoice 실패 (0x%08X)", hr);
            m_xaudio2.Reset();
            MFShutdown();
            m_mfStarted = false;
            return false;
        }

        m_masteringVoice->SetVolume(m_masterVolume);

        LOG_INFO("AudioManager 초기화 완료.");
        return true;
    }

    void AudioManager::Shutdown()
    {
        StopAll();

        if (m_bgmVoice)
        {
            DestroyVoice(m_bgmVoice);
            m_bgmVoice = nullptr;
        }
        m_bgmClip.reset();

        if (m_masteringVoice)
        {
            m_masteringVoice->DestroyVoice();
            m_masteringVoice = nullptr;
        }

        // IXAudio2 는 ComPtr 가 해제 (StopEngine 후 Release)
        if (m_xaudio2)
        {
            m_xaudio2->StopEngine();
            m_xaudio2.Reset();
        }

        if (m_mfStarted)
        {
            MFShutdown();
            m_mfStarted = false;
        }

        LOG_INFO("AudioManager 종료 완료.");
    }

    void AudioManager::Update()
    {
        if (m_oneShotVoices.empty())
            return;

        // 버퍼를 모두 소비한(재생이 끝난) one-shot 보이스를 정리한다.
        for (auto it = m_oneShotVoices.begin(); it != m_oneShotVoices.end();)
        {
            IXAudio2SourceVoice* voice = *it;
            XAUDIO2_VOICE_STATE state{};
            voice->GetState(&state, XAUDIO2_VOICE_NOSAMPLESPLAYED);

            if (state.BuffersQueued == 0)
            {
                DestroyVoice(voice);
                it = m_oneShotVoices.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    IXAudio2SourceVoice* AudioManager::CreateSourceVoice(const WAVEFORMATEX* format)
    {
        if (!m_xaudio2 || !format)
            return nullptr;

        IXAudio2SourceVoice* voice = nullptr;
        HRESULT hr = m_xaudio2->CreateSourceVoice(&voice, format);
        if (FAILED(hr))
        {
            LOG_ERROR("AudioManager - CreateSourceVoice 실패 (0x%08X)", hr);
            return nullptr;
        }
        return voice;
    }

    void AudioManager::DestroyVoice(IXAudio2SourceVoice* voice)
    {
        if (!voice)
            return;
        voice->Stop(0);
        voice->FlushSourceBuffers();
        voice->DestroyVoice();
    }

    void AudioManager::PlayOneShot(const std::shared_ptr<AudioClip>& clip,
                                   float volume, AudioCategory category,
                                   float pitch)
    {
        if (!m_xaudio2 || !clip || !clip->IsValid())
            return;

        IXAudio2SourceVoice* voice = CreateSourceVoice(clip->GetFormat());
        if (!voice)
            return;

        XAUDIO2_BUFFER buffer{};
        buffer.AudioBytes = clip->GetSize();
        buffer.pAudioData = clip->GetData();
        buffer.Flags      = XAUDIO2_END_OF_STREAM;

        HRESULT hr = voice->SubmitSourceBuffer(&buffer);
        if (FAILED(hr))
        {
            DestroyVoice(voice);
            return;
        }

        const float catVol = GetCategoryVolume(category);
        voice->SetVolume(volume * catVol);

        // Vary playback pitch. Clamp to XAudio2's safe default range (1/32 ~ 2.0).
        if (pitch < 0.03125f) pitch = 0.03125f;
        if (pitch > 2.0f)     pitch = 2.0f;
        voice->SetFrequencyRatio(pitch);

        voice->Start(0);

        // 재생이 끝나면 Update 에서 정리하도록 보관.
        m_oneShotVoices.push_back(voice);
    }

    void AudioManager::PlayBGM(const std::shared_ptr<AudioClip>& clip, bool loop, float volume)
    {
        if (!m_xaudio2 || !clip || !clip->IsValid())
            return;

        StopBGM();

        m_bgmVoice = CreateSourceVoice(clip->GetFormat());
        if (!m_bgmVoice)
            return;

        // 클립 메모리는 보이스가 살아있는 동안 유지되어야 한다.
        m_bgmClip = clip;

        XAUDIO2_BUFFER buffer{};
        buffer.AudioBytes = clip->GetSize();
        buffer.pAudioData = clip->GetData();
        buffer.Flags      = XAUDIO2_END_OF_STREAM;
        buffer.LoopCount  = loop ? XAUDIO2_LOOP_INFINITE : 0;

        HRESULT hr = m_bgmVoice->SubmitSourceBuffer(&buffer);
        if (FAILED(hr))
        {
            DestroyVoice(m_bgmVoice);
            m_bgmVoice = nullptr;
            m_bgmClip.reset();
            return;
        }

        m_bgmVoice->SetVolume(volume * m_bgmVolume);
        m_bgmVoice->Start(0);
    }

    void AudioManager::StopBGM()
    {
        if (!m_bgmVoice)
            return;
        DestroyVoice(m_bgmVoice);
        m_bgmVoice = nullptr;
        m_bgmClip.reset();
    }

    void AudioManager::PauseBGM()
    {
        if (m_bgmVoice)
            m_bgmVoice->Stop(0);
    }

    void AudioManager::ResumeBGM()
    {
        if (m_bgmVoice)
            m_bgmVoice->Start(0);
    }

    bool AudioManager::IsBGMPlaying() const
    {
        if (!m_bgmVoice)
            return false;
        XAUDIO2_VOICE_STATE state{};
        m_bgmVoice->GetState(&state, XAUDIO2_VOICE_NOSAMPLESPLAYED);
        return state.BuffersQueued > 0;
    }

    void AudioManager::SetMasterVolume(float volume)
    {
        m_masterVolume = std::clamp(volume, 0.0f, XAUDIO2_MAX_VOLUME_LEVEL);
        if (m_masteringVoice)
            m_masteringVoice->SetVolume(m_masterVolume);
    }

    void AudioManager::SetCategoryVolume(AudioCategory category, float volume)
    {
        volume = std::clamp(volume, 0.0f, XAUDIO2_MAX_VOLUME_LEVEL);
        switch (category)
        {
        case AudioCategory::BGM:
            m_bgmVolume = volume;
            if (m_bgmVoice)
                m_bgmVoice->SetVolume(m_bgmVolume);
            break;
        case AudioCategory::SFX:
            m_sfxVolume = volume;
            break;
        }
    }

    float AudioManager::GetCategoryVolume(AudioCategory category) const
    {
        switch (category)
        {
        case AudioCategory::BGM: return m_bgmVolume;
        case AudioCategory::SFX: return m_sfxVolume;
        }
        return 1.0f;
    }

    void AudioManager::StopAll()
    {
        for (IXAudio2SourceVoice* voice : m_oneShotVoices)
            DestroyVoice(voice);
        m_oneShotVoices.clear();

        StopBGM();
    }
}
