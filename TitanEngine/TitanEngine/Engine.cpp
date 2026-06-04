#include "pch.h"
#include "Engine.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Win32Window/IWindow.h"
#include "InputSystem/InputSystem.h"
#include "DebugConsole/DebugConsole.h"
#include "GameTimer.h"
#include "IRenderer.h"
#include "D2DRenderer.h"

using namespace Platform;
using namespace TitanEngine::SceneManagement;
using namespace TitanEngine::Time;

#define FIXED_TIMESTEP 0.02f    // 캐주얼 게임 기준

TitanEngine::Engine::Engine()
{
    m_timer = new GameTimer();
}

TitanEngine::Engine::~Engine()
{
    delete m_timer;
}

bool TitanEngine::Engine::Initialize(IWindow& window, const wchar_t* windowName, int width, int height)
{
	m_window = &window;
	void* handle = m_window->Create(windowName, width, height);

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

    if (false == SceneManager::Instance().Initialize())
    {
        LOG_ERROR("씬매니저가 초기화 되지 않았습니다.");
        return false;
    }

    m_renderer = new D2DRenderer(width, height, m_window->GetHWND());
    if (!m_renderer->Initialize())
        return false;

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
            // 프레임 시작
            InputSystem::Instance().FlushFrame(); 
            SceneManager::Instance().FlushFrame();

            // 이 프레임에서 사용될 씬 그래프 가져오기
            m_currentFrameActiveSceneGraph = SceneManager::Instance().GetActiveScene()->GetSceneGraph();
            if (!m_currentFrameActiveSceneGraph) continue;

            // 게임 타이머 시간 1틱 계산
            m_timer->Tick();
            m_fDeltaTime = m_timer->DeltaTime();   // 초 단위로 통일
            m_fFrameCount += m_fDeltaTime;

            // 누적 프레임이 물리 계산할 시간 기준점을 넘었으면 물리 연산 실행
            while (m_fFrameCount >= FIXED_TIMESTEP)
            {
                FixedUpdate(FIXED_TIMESTEP);
                m_fFrameCount = 0;
            }

            Update(m_fDeltaTime);
            LateUpdate(m_fDeltaTime);
            Render();
        }
    }
}

void TitanEngine::Engine::Finalize()
{
    SceneManager::Instance().Shutdown();
    m_renderer->ShutDown();
}

void TitanEngine::Engine::FixedUpdate(float fixedTime)
{
    m_currentFrameActiveSceneGraph->FixedUpdate(fixedTime);
}

void TitanEngine::Engine::Update(float deltaTime)
{
    m_currentFrameActiveSceneGraph->Update(deltaTime);
}

void TitanEngine::Engine::LateUpdate(float deltaTime)
{
    m_currentFrameActiveSceneGraph->LateUpdate(deltaTime);
}

void TitanEngine::Engine::Render()
{
    // RenderSystem 연결 예정
    if (m_renderer == nullptr)
        return;

    m_renderer->RenderBegin();
    m_renderer->DrawCircle(600, 600, 30, D2D1::ColorF::Tomato);
    m_renderer->RenderEnd();
}