#include "pch.h"
#include "Engine.h"
#include "SceneManager.h"
#include "TestScene.h"
#include "Win32Window/IWindow.h"
#include "Win32Window/Win32Window.h"
#include "InputSystem/InputSystem.h"
#include "DebugConsole/DebugConsole.h"
#include "GameTimer.h"
#include "D2DRenderer.h"
#include "InGameScene.h"

using namespace Platform;
using namespace TitanEngine::Renderer;
using namespace TitanEngine::SceneManagement;
using namespace TitanEngine::Time;

#define FIXED_TIMESTEP 0.02f    // 캐주얼 게임 기준

TitanEngine::Engine::Engine()
{
    m_timer = new GameTimer();
    m_renderer = new D2DRenderer();
}

TitanEngine::Engine::~Engine()
{
    delete m_renderer;
    delete m_timer;
}

bool TitanEngine::Engine::Initialize(IWindow& window, const wchar_t* windowName, int width, int height)
{
	m_window = &window;
	void* handle = m_window->Create(windowName, width, height);
    auto* wnd = static_cast<Win32Window*>(m_window);

    if (!handle)
    {
        LOG_ERROR("윈도우 핸들이 할당되지 않았습니다.");
        return false;
    }

    if (false == InputSystem::Instance().Initialize(m_window->GetHWND()))
    {
        LOG_ERROR("인풋 시스템이 초기화 되지 않았습니다.");
        return false;
    }
    InputSystem::Instance().SetDebuging(false);

    if (false == SceneManager::Instance().Initialize())
    {
        LOG_ERROR("씬매니저가 초기화 되지 않았습니다.");
        return false;
    }

    if (false == m_renderer->Initialize(m_window->GetHWND()))
    {
        LOG_ERROR("D2D 렌더 시스템이 초기화 되지 않았습니다.");
        return false;
    }

    // 씬 등록 + 전환 예약  ← 추가
    auto testScene = std::make_shared<TestScene>("TestScene");
    SceneManager::Instance().RegisterScene("TestScene", testScene);
    SceneManager::Instance().LoadScene("TestScene");

    auto inGameScene = std::make_shared<InGameScene>("InGameScene");
    SceneManager::Instance().RegisterScene("InGameScene", inGameScene);
    SceneManager::Instance().LoadScene("InGameScene");

    wnd->AddObserver(WM_SIZE, m_renderer);
    m_timer->Reset();

    LOG_DEBUG("엔진이 성공적으로 초기화 되었습니다.");
	return true;
}

void TitanEngine::Engine::Run()
{
	MSG msg = { 0 };
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (!InputSystem::Instance().OnHandleMessage(msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            // 게임 타이머 시간 1틱 계산
            m_timer->Tick();
            m_fDeltaTime = m_timer->DeltaTime();   // 초 단위로 통일
            m_fFrameCount += m_fDeltaTime;

            // 프레임 시작
            InputSystem::Instance().FlushFrame(); 
            SceneManager::Instance().FlushFrame();

            // 이 프레임에서 사용될 씬 그래프 가져오기
            m_currentFrameActiveScene = SceneManager::Instance().GetActiveScene();
            if (!m_currentFrameActiveScene) continue;

            // 누적 프레임이 물리 계산할 시간 기준점을 넘었으면 물리 연산 실행
            while (m_fFrameCount >= FIXED_TIMESTEP)
            {
                FixedUpdate(FIXED_TIMESTEP);
                m_fFrameCount -= FIXED_TIMESTEP;
            }

            Update(m_fDeltaTime);
            LateUpdate(m_fDeltaTime);
            Render();
        }
    }
}

void TitanEngine::Engine::Finalize()
{
    SceneManager::Instance().UnInitialize();
    m_renderer->UnInitialize();
}

void TitanEngine::Engine::FixedUpdate(float fixedTime)
{
    m_currentFrameActiveScene->FixedUpdate(fixedTime);
}

void TitanEngine::Engine::Update(float deltaTime)
{
    m_currentFrameActiveScene->PropagateWorldMatrix();
    m_currentFrameActiveScene->Update(deltaTime);
}

void TitanEngine::Engine::LateUpdate(float deltaTime)
{
    m_currentFrameActiveScene->LateUpdate(deltaTime);
}

void TitanEngine::Engine::Render()
{
    m_renderer->RenderBegin();
    m_currentFrameActiveScene->Render(
        m_renderer->GetContext(),
        (float)m_renderer->GetWidth(),
        (float)m_renderer->GetHeight()
    );
#ifdef _DEBUG
    float fps = (m_fDeltaTime > 0.0f) ? 1.0f / m_fDeltaTime : 0.0f;
    m_renderer->ShowFPS(fps);
#endif
    m_renderer->RenderEnd();
}