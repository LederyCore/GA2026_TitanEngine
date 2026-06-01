#include "pch.h"
#include "WindowFactory.h"
#include "Win32Window.h"

using namespace Platform;

// 정적 윈도우 프로시저 (메시지 라우터)
LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_CREATE)
    {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreate->lpCreateParams));
    }

    IWindow* pWindow = reinterpret_cast<IWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if (pWindow)
    {
        return pWindow->HandleMessage(hWnd, message, wParam, lParam);
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

std::unique_ptr<IWindow> Platform::WindowFactory::Create(const std::wstring& windowName, int width, int height)
{
    // 1. 객체 생성 (HWND는 나중에 설정)
    auto wnd = std::make_unique<Win32Window>(nullptr);

    // 2. 클래스 등록
    const wchar_t* className = L"Win32Window";
    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.lpfnWndProc = StaticWndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = className;

    RegisterClassEx(&wc);

    // 3. 해상도 보정 (Client Area 유지)
    RECT rc = { 0, 0, width, height };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    // 4. 윈도우 생성 (객체 포인터를 lpParam으로 전달)
    HWND hWnd = CreateWindowEx(
        0, className, windowName.c_str(), WS_OVERLAPPEDWINDOW,
        (GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2,
        (GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2,
        rc.right - rc.left, rc.bottom - rc.top,
        nullptr, nullptr, GetModuleHandle(nullptr),
        wnd.get()
    );

    if (!hWnd) return nullptr;

    wnd->SetHWND(hWnd);
    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    return wnd;
}