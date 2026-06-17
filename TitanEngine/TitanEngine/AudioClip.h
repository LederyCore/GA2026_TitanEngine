#pragma once
#include "Resource.h"
#include <vector>
#include <mmreg.h>   // WAVEFORMATEX

namespace TitanEngine
{
    // wav / mp3 파일을 디코딩하여 PCM 데이터를 보관하는 오디오 리소스.
    // Texture2D 가 WIC 로 이미지를 디코딩하듯, AudioClip 은 Media Foundation 으로
    // 오디오를 디코딩한다. ResourceManager::Load<AudioClip>(path) 로 캐싱되어 로드된다.
    class AudioClip : public Resource
    {
    public:
        // Resource 인터페이스 구현.
        // ctx(D2D 컨텍스트)는 오디오 디코딩에 필요 없으므로 사용하지 않는다.
        bool Load(ID2D1DeviceContext7* ctx, const std::filesystem::path& path) override;

        // XAudio2 SourceVoice 생성에 사용할 포맷.
        const WAVEFORMATEX* GetFormat() const
        {
            return m_format.empty() ? nullptr
                : reinterpret_cast<const WAVEFORMATEX*>(m_format.data());
        }

        // 디코딩된 PCM 버퍼.
        const BYTE* GetData()  const { return m_pcm.data(); }
        UINT32      GetSize()  const { return static_cast<UINT32>(m_pcm.size()); }

        double      GetDurationSec() const { return m_durationSec; }
        bool        IsValid()  const { return !m_pcm.empty() && !m_format.empty(); }

    private:
        std::vector<BYTE> m_format;        // WAVEFORMATEX(+확장 바이트)
        std::vector<BYTE> m_pcm;           // 디코딩된 PCM 샘플 데이터
        double            m_durationSec = 0.0;
    };
}
