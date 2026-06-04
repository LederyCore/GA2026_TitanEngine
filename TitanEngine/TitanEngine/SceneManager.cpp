#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"
#include "DebugConsole/DebugConsole.h"
#include "TestSceneLES.h"

namespace TitanEngine::SceneManagement
{
    bool SceneManager::Initialize()
    {
        // 에디터 없으므로 사용할 씬 수동 등록 (의도된 하드코딩)
        if (!RegisterScene("TitleScene", std::make_shared<TestSceneLES>("TitleScene")))
            return false;

        // 첫 씬 예약
        LoadScene("TitleScene");

        return true;
    }

    void SceneManager::Shutdown()
    {
        // 현재 씬 정리
        if (m_currentScene)
        {
            m_currentScene->OnUnLoad();
            m_currentScene = nullptr;
        }

        // 예약 초기화
        m_nextSceneName.clear();

        // 대기 중인 제거 큐 비우기
        while (!m_removeQueue.empty())
            m_removeQueue.pop();

        // 씬 맵 전체 정리
        m_scenes.clear();
    }

    // ── 씬 전환 예약 ──────────────────────────────────────────────
    void SceneManager::LoadScene(const std::string& sceneName)
    {
        auto it = m_scenes.find(sceneName);
        if (it == m_scenes.end())
        {
            LOG_ERROR("Scene '%s' 을(를) 찾을 수 없습니다.", sceneName.c_str());
            return;
        }

        // 이미 같은 씬 예약 중이면 무시
        if (m_nextSceneName == sceneName)
        {
            LOG_ERROR("Scene '%s' 이미 전환 예약 중입니다.", sceneName.c_str());
            return;
        }

        m_nextSceneName = sceneName;
    }

    // ── 씬 제거 예약 ──────────────────────────────────────────────
    void SceneManager::UnLoadScene(const std::string& sceneName)
    {
        // 현재 활성 씬은 바로 제거 불가
        if (m_currentScene && sceneName == m_currentScene->GetSceneName())
        {
            LOG_ERROR("현재 실행 중인 씬 '%s'은(는) UnLoad할 수 없습니다.", sceneName.c_str());
            return;
        }

        // 등록된 씬인지 확인
        if (m_scenes.find(sceneName) == m_scenes.end())
        {
            LOG_ERROR("Scene '%s' 을(를) 찾을 수 없습니다.", sceneName.c_str());
            return;
        }

        // 이미 큐에 있는지 중복 확인
        std::queue<std::string> temp = m_removeQueue;
        while (!temp.empty())
        {
            if (temp.front() == sceneName)
            {
                LOG_ERROR("Scene '%s' 이미 제거 예약 중입니다.", sceneName.c_str());
                return;
            }
            temp.pop();
        }

        m_removeQueue.push(sceneName);
    }

    // ── 프레임 경계에서 실행 ──────────────────────────────────────
    void SceneManager::FlushFrame()
    {
        // 1. 제거 큐 처리 (씬 전환보다 먼저)
        while (!m_removeQueue.empty())
        {
            const std::string& name = m_removeQueue.front();

            // 혹시 전환 예약된 씬이 제거 대상이면 전환 취소
            if (m_nextSceneName == name)
            {
                LOG_ERROR("전환 예약된 씬 '%s'이 제거 대상입니다. 전환을 취소합니다.", name.c_str());
                m_nextSceneName.clear();
            }

            if (m_scenes.erase(name) > 0)
                LOG_INFO("Scene '%s' 제거 완료.", name.c_str());
            else
                LOG_ERROR("Scene '%s' 제거 실패.", name.c_str());

            m_removeQueue.pop();
        }

        // 2. 씬 전환 처리
        if (m_nextSceneName.empty())
            return;

        auto it = m_scenes.find(m_nextSceneName);
        if (it == m_scenes.end())
        {
            LOG_ERROR("Scene '%s' 전환 실패: 씬을 찾을 수 없습니다.", m_nextSceneName.c_str());
            m_nextSceneName.clear();
            return;
        }

        // 3. 현재 씬 정리
        if (m_currentScene)
        {
            m_currentScene->OnUnLoad();
            m_currentScene = nullptr;
        }

        // 4. 새 씬 활성화
        m_currentScene = it->second;
        m_currentScene->OnLoad();

        LOG_INFO("Scene '%s' 전환 완료.", m_nextSceneName.c_str());

        // 5. 예약 초기화
        m_nextSceneName.clear();
    }

    // ── 씬 등록 / 해제 ────────────────────────────────────────────
    bool SceneManager::RegisterScene(const std::string& sceneName, std::shared_ptr<Scene> scene)
    {
        if (!scene)
        {
            LOG_ERROR("Scene '%s' 등록 실패: nullptr입니다.", sceneName.c_str());
            return false;
        }

        auto result = m_scenes.insert({ sceneName, scene });
        if (!result.second)
        {
            LOG_ERROR("Scene '%s' 등록 실패: 이미 존재하는 이름입니다.", sceneName.c_str());
            return false;
        }

        LOG_INFO("Scene '%s' 등록 완료.", sceneName.c_str());
        return true;
    }

    bool SceneManager::UnRegisterScene(const std::string& sceneName)
    {
        // 현재 활성 씬 보호
        if (m_currentScene && sceneName == m_currentScene->GetSceneName())
        {
            LOG_ERROR("현재 실행 중인 씬 '%s'은(는) 삭제할 수 없습니다.", sceneName.c_str());
            return false;
        }

        if (m_scenes.erase(sceneName) == 0)
        {
            LOG_ERROR("Scene '%s' 제거 실패: 등록되지 않은 씬입니다.", sceneName.c_str());
            return false;
        }

        LOG_INFO("Scene '%s' 등록 해제 완료.", sceneName.c_str());
        return true;
    }
}