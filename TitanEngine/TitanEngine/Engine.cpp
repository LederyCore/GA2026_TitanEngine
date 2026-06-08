#include "pch.h"
#include "Engine.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Win32Window/IWindow.h"
#include "InputSystem/InputSystem.h"
#include "DebugConsole/DebugConsole.h"
#include "GameTimer.h"
#include "D2DRenderer.h"

using namespace Platform;
using namespace TitanEngine::Renderer;
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
    InputSystem::Instance().SetDebuging(false);

    if (false == SceneManager::Instance().Initialize())
    {
        LOG_ERROR("씬매니저가 초기화 되지 않았습니다.");
        return false;
    }

    m_renderer = new D2DRenderer(width, height, m_window->GetHWND());
    if (!m_renderer->Initialize())
    {
        LOG_ERROR("D2D 렌더 시스템이 초기화 되지 않았습니다.");
        return false;
    }

    m_renderer->CreateBitmapFromFile(L"./Resource/cat.png", *m_bitmapCat.GetAddressOf());



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
            // SceneGraph가 유효한지 체크
            if (!m_currentFrameActiveScene->GetSceneGraph()) continue;

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
    SceneManager::Instance().Shutdown();
    m_renderer->ShutDown();
}

void TitanEngine::Engine::FixedUpdate(float fixedTime)
{
    // IPhysics 구현한 것만 물리 연산 수행
    m_currentFrameActiveScene->GetPhysicsSystem()->FixedUpdate(fixedTime);

    // 컴포넌트의 FixedUpdate 오버라이드한 것만 물리 결과 기반 게임 로직
    m_currentFrameActiveScene->GetUpdateSystem()->FixedUpdate(fixedTime);
}

void TitanEngine::Engine::Update(float deltaTime)
{
    m_currentFrameActiveScene->GetSceneGraph()->PropagateWorldMatrix();

    m_currentFrameActiveScene->GetUpdateSystem()->Update(deltaTime);
}

void TitanEngine::Engine::LateUpdate(float deltaTime)
{
    m_currentFrameActiveScene->GetUpdateSystem()->LateUpdate(deltaTime);
}

void TitanEngine::Engine::Render()
{
    m_renderer->RenderBegin();

    #ifdef _DEBUG
        float fps = 1.0f / m_fDeltaTime;
        m_renderer->ShowFPS(fps);
    #endif // _DEBUG
    
        // TEST
        m_renderer->DrawCircle(600, 600, 30, D2D1::ColorF::Tomato);

        D2D1_RECT_F dest = D2D1::RectF(0, 0, 200, 200);
        m_renderer->DrawBitmap(m_bitmapCat.Get(), dest);

        // RenderSystem에 DeviceContext 넘겨서 일괄 드로우
		m_renderer->RenderScene(m_currentFrameActiveScene->GetRenderSystem());
    m_renderer->RenderEnd();
}