#pragma once
#include <xaudio2.h>
#include <wrl/client.h>
#include <memory>
#include <vector>

using Microsoft::WRL::ComPtr;

namespace TitanEngine
{
    class AudioClip;

    // 사운드 분류. 카테고리별로 볼륨을 따로 제어할 수 있다.
    enum class AudioCategory
    {
        BGM,
        SFX,
    };

    // XAudio2 오디오 엔진을 소유하고 사운드 재생을 총괄하는 매니저.
    // ResourceManager / SceneManager 와 동일한 싱글톤 패턴을 따른다.
    //
    //  - BGM   : PlayBGM/StopBGM 으로 단일 배경음을 루프 재생/교체
    //  - 효과음 : PlayOneShot 으로 다중 동시 재생 (재생이 끝나면 자동 정리)
    //  - 개별 오브젝트 사운드는 AudioComponent 가 CreateSourceVoice 로 직접 보이스를 만든다.
    class AudioManager
    {
    public:
        static AudioManager& Instance()
        {
            static AudioManager instance;
            return instance;
        }
        AudioManager(const AudioManager&) = delete;
        AudioManager& operator=(const AudioManager&) = delete;

        // XAudio2 엔진 + 마스터링 보이스 생성, Media Foundation 시작.
        // 오디오 클립을 로드하기 전(보통 Engine 초기화 시)에 호출해야 한다.
        bool Initialize();
        void Shutdown();
        bool IsInitialized() const { return m_xaudio2 != nullptr; }

        // 매 프레임 호출 — 재생이 끝난 one-shot 보이스를 정리한다.
        void Update();

        // AudioComponent 등이 직접 SourceVoice 를 생성할 때 사용.
        // 반환된 보이스는 호출자가 DestroyVoice() 로 해제할 책임을 가진다.
        IXAudio2SourceVoice* CreateSourceVoice(const WAVEFORMATEX* format);

        // ─── 효과음(fire-and-forget) ──────────────────────────────
        // 클립을 즉시 1회 재생한다. 같은 클립을 겹쳐 재생할 수 있다.
        // pitch = playback frequency ratio (1.0 = original, 0.5~2.0 recommended).
        void PlayOneShot(const std::shared_ptr<AudioClip>& clip,
                         float volume = 1.0f,
                         AudioCategory category = AudioCategory::SFX,
                         float pitch = 1.0f);

        // ─── 배경음(BGM) ─────────────────────────────────────────
        void PlayBGM(const std::shared_ptr<AudioClip>& clip,
                     bool loop = true, float volume = 1.0f);
        void StopBGM();
        void PauseBGM();
        void ResumeBGM();
        bool IsBGMPlaying() const;

        // ─── 볼륨 (0.0 ~ 1.0) ────────────────────────────────────
        void  SetMasterVolume(float volume);
        float GetMasterVolume() const { return m_masterVolume; }

        void  SetCategoryVolume(AudioCategory category, float volume);
        float GetCategoryVolume(AudioCategory category) const;

        // 모든 사운드를 즉시 멈춘다.
        void StopAll();

    private:
        AudioManager() = default;
        ~AudioManager() = default;

        void DestroyVoice(IXAudio2SourceVoice* voice);

    private:
        ComPtr<IXAudio2>        m_xaudio2;
        IXAudio2MasteringVoice* m_masteringVoice = nullptr;

        // 재생 중인 one-shot 효과음 보이스 (끝나면 Update 에서 정리)
        std::vector<IXAudio2SourceVoice*> m_oneShotVoices;

        // 전용 BGM 보이스
        IXAudio2SourceVoice*    m_bgmVoice = nullptr;
        std::shared_ptr<AudioClip> m_bgmClip;   // PCM 메모리 수명 유지용

        bool  m_mfStarted    = false;
        float m_masterVolume = 1.0f;
        float m_bgmVolume    = 1.0f;
        float m_sfxVolume    = 1.0f;
    };
}
