#include "pch.h"
#include "AudioClip.h"
#include "AudioManager.h"
#include "DebugConsole/DebugConsole.h"

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>

#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

namespace TitanEngine
{
    bool AudioClip::Load(ID2D1DeviceContext7* /*ctx*/, const std::filesystem::path& path)
    {
        // Media Foundation 은 AudioManager 가 MFStartup 으로 초기화한다.
        // 오디오 클립을 로드하기 전에 AudioManager 가 초기화되어 있어야 한다.
        if (!AudioManager::Instance().IsInitialized())
        {
            LOG_ERROR("AudioClip::Load - AudioManager 가 초기화되지 않았습니다. (%ls)", path.c_str());
            return false;
        }

        // 1) 소스 리더 생성 (wav/mp3 등 파일 확장자에 맞춰 디코더 자동 선택)
        ComPtr<IMFSourceReader> reader;
        HRESULT hr = MFCreateSourceReaderFromURL(path.wstring().c_str(), nullptr, &reader);
        if (FAILED(hr))
        {
            LOG_ERROR("AudioClip::Load - 소스 리더 생성 실패 (0x%08X) %ls", hr, path.c_str());
            return false;
        }

        // 2) 출력 포맷을 비압축 PCM 으로 강제 → mp3 도 PCM 으로 디코딩된다.
        ComPtr<IMFMediaType> pcmType;
        hr = MFCreateMediaType(&pcmType);
        if (FAILED(hr)) return false;

        pcmType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
        pcmType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);

        // 첫 번째 오디오 스트림만 사용
        reader->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS, FALSE);
        reader->SetStreamSelection(MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);

        hr = reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pcmType.Get());
        if (FAILED(hr))
        {
            LOG_ERROR("AudioClip::Load - PCM 출력 포맷 설정 실패 (0x%08X) %ls", hr, path.c_str());
            return false;
        }

        // 3) 실제로 협상된 출력 미디어 타입에서 WAVEFORMATEX 추출
        ComPtr<IMFMediaType> actualType;
        hr = reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &actualType);
        if (FAILED(hr)) return false;

        WAVEFORMATEX* wfx = nullptr;
        UINT32 wfxSize = 0;
        hr = MFCreateWaveFormatExFromMFMediaType(actualType.Get(), &wfx, &wfxSize);
        if (FAILED(hr) || !wfx)
        {
            LOG_ERROR("AudioClip::Load - WAVEFORMATEX 변환 실패 (0x%08X) %ls", hr, path.c_str());
            return false;
        }

        m_format.assign(reinterpret_cast<BYTE*>(wfx),
                        reinterpret_cast<BYTE*>(wfx) + wfxSize);
        CoTaskMemFree(wfx);

        // 4) 샘플을 끝까지 읽어 PCM 버퍼에 누적
        m_pcm.clear();
        for (;;)
        {
            DWORD streamFlags = 0;
            ComPtr<IMFSample> sample;
            hr = reader->ReadSample(
                MF_SOURCE_READER_FIRST_AUDIO_STREAM,
                0, nullptr, &streamFlags, nullptr, &sample);

            if (FAILED(hr))
            {
                LOG_ERROR("AudioClip::Load - ReadSample 실패 (0x%08X) %ls", hr, path.c_str());
                return false;
            }

            if (streamFlags & MF_SOURCE_READERF_ENDOFSTREAM)
                break;

            if (!sample)   // 일시적으로 샘플이 없을 수 있음
                continue;

            ComPtr<IMFMediaBuffer> buffer;
            hr = sample->ConvertToContiguousBuffer(&buffer);
            if (FAILED(hr)) return false;

            BYTE* data = nullptr;
            DWORD dataSize = 0;
            hr = buffer->Lock(&data, nullptr, &dataSize);
            if (FAILED(hr)) return false;

            m_pcm.insert(m_pcm.end(), data, data + dataSize);
            buffer->Unlock();
        }

        if (m_pcm.empty())
        {
            LOG_ERROR("AudioClip::Load - 디코딩된 오디오 데이터가 없습니다. %ls", path.c_str());
            return false;
        }

        // 5) 재생 길이 계산
        const WAVEFORMATEX* fmt = GetFormat();
        if (fmt && fmt->nAvgBytesPerSec > 0)
            m_durationSec = static_cast<double>(m_pcm.size()) / fmt->nAvgBytesPerSec;

        LOG_INFO("AudioClip 로드 완료: %ls (%.2fs, %u Hz, %u ch)",
                 path.c_str(), m_durationSec,
                 fmt ? fmt->nSamplesPerSec : 0,
                 fmt ? fmt->nChannels : 0);

        return true;
    }
}
