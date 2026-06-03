#include "pch.h"
#include "Engine.h"
#include "Win32Window/IWindow.h"
#include "InputSystem/InputSystem.h"

using namespace Platform;

TitanEngine::Engine::~Engine()
{

}

bool TitanEngine::Engine::Initialize(IWindow& window, const wchar_t* windowName, int width, int height)
{
	m_window = &window;
	void* handle = m_window->Create(windowName, width, height);
    if (!handle)
    {
        return false;
    }

    if (false == InputSystem::Instance().Initialize(m_window->GetHWND()))
    {
        return false;
    }

	return true;
}

void TitanEngine::Engine::Run()
{
	MSG msg = { 0 };
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (!Platform::InputSystem::Instance().OnHandleMessage(msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            // 프레임 시작
            Platform::InputSystem::Instance().FlushFrame(); 
            Update();
            Render();
        }
    }

}

void TitanEngine::Engine::Finalize()
{

}

void TitanEngine::Engine::Update()
{
}

void TitanEngine::Engine::Render()
{

}
