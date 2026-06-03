#include "pch.h"
#include "Engine.h"
#include "Win32Window/IWindow.h"
#include "InputSystem/InputSystem.h"
#include "DebugConsole/DebugConsole.h"
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

    if (!InitD2DRenderSystem())
		return false;

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

bool TitanEngine::Engine::InitD2DRenderSystem()
{

    // D3D11 디바이스 생성
	ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;

    D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_0 };
    D3D_FEATURE_LEVEL d3dFeatureLevel;

    HRESULT hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        featureLevels,
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        &device,
        &d3dFeatureLevel,
        &context);

    if (FAILED(hr)) 
		return false;



    return true;
}
