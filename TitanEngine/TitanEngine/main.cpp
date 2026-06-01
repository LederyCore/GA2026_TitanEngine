#include "pch.h"
#include <iostream>

// =============================================================================
// FILE:         main.cpp
// DESCRIPTION:  이 엔진프로그램의 시작점 역할을 합니다. 
// =============================================================================
//
// [책임 구현 / Responsibility Implementation]
//
//   <아는 것 / Knowing>
//     - validate()     : 결제 수단과 한도를 기반으로 승인 가능 여부 계산
//     - getLimit()     : 현재 설정된 결제 한도 반환
//
//   <하는 것 / Doing>
//     - approve()      : 외부 PG 객체에 실제 승인 요청 위임
//     - createReceipt(): ReceiptFactory를 통해 영수증 객체 생성
//
//
// [Notes]
//   - 알아두면 좋은 특이사항, 제약 조건, 레거시 이유 등
//   - TODO: 향후 개선이 필요한 항목
// -----------------------------------------------------------------------------
// AUTHOR:       이은수
// CREATED:      2026-06-01
// UPDATED:      2026-06-01
// =============================================================================

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#ifdef _DEBUG
int main()
{
    std::cout << "현재 모드는 테스트 모드 입니다.";

    return 0;
}
#elif NDEBUG
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    std::cout << "현재 모드는 테스트 모드 입니다.";

    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Learn to Program Windows",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
#endif

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // All painting occurs here, between BeginPaint and EndPaint.

        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hwnd, &ps);
    }
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}