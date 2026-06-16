#include "pch.h"
#include "Engine.h"
#include "Button.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "TestScene.h"
#include "Win32Window/IWindow.h"
#include "Win32Window/Win32Window.h"
#include "InputSystem/InputSystem.h"
#include "DebugConsole/DebugConsole.h"
#include "GameTimer.h"
#include "D2DRenderer.h"
#include "Bubble.h"
#include "InGameScene.h"
#include "TitleScene.h"

using namespace Platform;
using namespace TitanEngine::Renderer;
using namespace TitanEngine::SceneManagement;
using namespace TitanEngine::Time;

#define FIXED_TIMESTEP 0.02f    

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
        LOG_ERROR("윈도우 핸들이 초기화 되지 않았습니다.");
        return false;
    }

    if (false == InputSystem::Instance().Initialize(m_window->GetHWND()))
    {
        LOG_ERROR("인풋 시스템이 초기화 되지 않았습니다.");
        return false;
    }
    InputSystem::Instance().SetDebuging(false);

    if (false == m_renderer->Initialize(m_window->GetHWND()))
    {
        LOG_ERROR("D2D 렌더링 시스템이 초기화 되지 않았습니다.");
        return false;
    }

    if (false == ResourceManager::Instance().Initialize(m_renderer->GetContext()))
    {
        LOG_ERROR("리소스 매니저가 초기화 되지 않았습니다.");
        return false;
    }

    if (false == SceneManager::Instance().Initialize())
    {
        LOG_ERROR("씬 매니저가 초기화 되지 않았습니다.");
        return false;
    }

   /* auto testScene = std::make_shared<TestScene>("TestScene");
    SceneManager::Instance().RegisterScene("TestScene", testScene);
    SceneManager::Instance().LoadScene("TestScene");*/

    auto titleScene = std::make_shared<TitleScene>("TitleScene");
    SceneManager::Instance().RegisterScene("TitleScene", titleScene);

    auto inGameScene = std::make_shared<InGameScene>("InGameScene");
    SceneManager::Instance().RegisterScene("InGameScene", inGameScene);


    SceneManager::Instance().LoadScene("TitleScene");


    wnd->AddObserver(WM_SIZE, m_renderer);
    wnd->AddObserver(WM_EXITSIZEMOVE, m_renderer);
    m_timer->Reset();

    
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
            m_timer->Tick();
            m_fDeltaTime = m_timer->DeltaTime();   
            m_fFrameCount += m_fDeltaTime;

            SceneManager::Instance().FlushFrame();


            m_currentFrameActiveScene = SceneManager::Instance().GetActiveScene();
            if (!m_currentFrameActiveScene) continue;


            while (m_fFrameCount >= FIXED_TIMESTEP)
            {
                FixedUpdate(FIXED_TIMESTEP);
                m_fFrameCount -= FIXED_TIMESTEP;
            }

            Update(m_fDeltaTime);
            LateUpdate(m_fDeltaTime);
            Render();
            InputSystem::Instance().FlushFrame(); 
        }
    }
}

void TitanEngine::Engine::Finalize()
{
    SceneManager::Instance().UnInitialize();
    m_renderer->FlushAndClearTarget();
    ResourceManager::Instance().UnLoadAll();
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
    // 히트 테스트에 필요한 화면 크기 동기화 (리사이즈 대응)
    float w = (float)m_renderer->GetWidth();
    float h = (float)m_renderer->GetHeight();
    Button::SetScreenSize(w, h);
    Bubble::SetScreenSize(w, h);

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