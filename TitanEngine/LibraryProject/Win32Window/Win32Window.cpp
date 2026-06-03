#include "pch.h"
#include <windowsx.h>
#include "Win32Window.h"

Platform::Win32Window::~Win32Window()
{
	Close();
}

HWND Platform::Win32Window::Create(const std::wstring& windowName, int width, int height)
{
	const wchar_t* className = L"Win32Window";
	WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
	wc.lpfnWndProc = StaticWndProc;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.lpszClassName = className;

	m_width = width;
	m_height = height;

	if (!RegisterClassEx(&wc))
	{
		if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
			return nullptr;
	}

	RECT rc = { 0, 0, width, height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND hWnd = CreateWindowEx(
		0, className, windowName.c_str(), WS_OVERLAPPEDWINDOW,
		(GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2,
		rc.right - rc.left, rc.bottom - rc.top,
		nullptr, nullptr, GetModuleHandle(nullptr),
		this
	);

	if (!hWnd) return nullptr;
	Show();

	return hWnd;
}

void Platform::Win32Window::Show()
{
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);
}

void Platform::Win32Window::Close()
{
	if (m_hWnd != nullptr)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = nullptr;
	}
}

LRESULT Platform::Win32Window::StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Win32Window* pWindow = nullptr;

	if (msg == WM_NCCREATE)
	{
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		pWindow = reinterpret_cast<Win32Window*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
		pWindow->m_hWnd = hWnd;

		DefWindowProc(hWnd, msg, wParam, lParam); // NC 영역 내부 처리
		return TRUE;                              // 반드시 TRUE 반환
	}

	pWindow = reinterpret_cast<Win32Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if (pWindow)
		return pWindow->HandleMessage(hWnd, msg, wParam, lParam);

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void Platform::Win32Window::AddObserver(IWindowObserver* obs)
{
	AddObserver(0, obs);
}

void Platform::Win32Window::RemoveObserver(IWindowObserver* obs)
{
	RemoveObserver(0, obs);
}

void Platform::Win32Window::AddObserver(UINT msg, IWindowObserver* obs)
{
	auto& observers = m_observerMap[msg];
	if (std::find(observers.begin(), observers.end(), obs) == observers.end())
	{
		observers.push_back(obs);
	}
}

void Platform::Win32Window::RemoveObserver(UINT msg, IWindowObserver* obs)
{
	auto it = m_observerMap.find(msg);
	if (it != m_observerMap.end())
	{
		auto& list = it->second;
		list.erase(std::remove(list.begin(), list.end(), obs), list.end());
	}
}

LRESULT Platform::Win32Window::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// 입력관련 이벤트 전달
	Notify(msg, wParam, lParam);

	// 윈도우 창 관련 메시지만 여기서 처리
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void Platform::Win32Window::Notify(UINT msg, WPARAM wParam, LPARAM lParam)
{

	// A. 전체 구독자(0번)에게 먼저 통지
	if (m_observerMap.count(0)) 
	{
		for (auto* obs : m_observerMap[0]) 
		{
			// 필요에 따라 전체 알림용 가상 함수를 옵저버에 추가할 수 있습니다.
		}
	}

	// B. 특정 메시지 구독자에게 통지
	auto it = m_observerMap.find(msg);
	if (it != m_observerMap.end())
	{
		const auto& observers = it->second; // 복사본
		for (const auto& obs : observers)
		{
			switch (msg)
			{
				// ── 키보드 ──────────────────────────────────────────
			case WM_KEYDOWN:        obs->OnKeyDown(wParam); break;
			case WM_KEYUP:          obs->OnKeyUp(wParam); break;
			case WM_SYSKEYDOWN:     obs->OnSysKeyDown(wParam); break;
			case WM_SYSKEYUP:       obs->OnSysKeyUp(wParam); break;
			case WM_CHAR:           obs->OnChar(wParam); break;

				// ── 마우스 ──────────────────────────────────────────
			case WM_MOUSEMOVE:      obs->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
			case WM_LBUTTONDOWN:    obs->OnMouseLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
			case WM_LBUTTONUP:      obs->OnMouseLButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
			case WM_RBUTTONDOWN:    obs->OnMouseRButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
			case WM_RBUTTONUP:      obs->OnMouseRButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
			case WM_MBUTTONDOWN:    obs->OnMouseMButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
			case WM_MBUTTONUP:      obs->OnMouseMButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
			case WM_MOUSEWHEEL:     obs->OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam)); break;
			case WM_XBUTTONDOWN:    obs->OnMouseXButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), GET_XBUTTON_WPARAM(wParam)); break;
			case WM_XBUTTONUP:      obs->OnMouseXButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), GET_XBUTTON_WPARAM(wParam)); break;
			case WM_LBUTTONDBLCLK:  obs->OnMouseLButtonDblClk(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;

				// ── 윈도우 상태 ─────────────────────────────────────
			case WM_SIZE:           obs->OnResize(LOWORD(lParam), HIWORD(lParam)); break;
			case WM_MOVE:           obs->OnMove(LOWORD(lParam), HIWORD(lParam)); break;
			case WM_ACTIVATE:       obs->OnActivate(LOWORD(wParam) != WA_INACTIVE); break;
			case WM_SETFOCUS:       obs->OnFocusGained(); break;
			case WM_KILLFOCUS:      obs->OnFocusLost(); break;
			case WM_ENTERSIZEMOVE:  obs->OnEnterSizeMove(); break;
			case WM_EXITSIZEMOVE:   obs->OnExitSizeMove(); break;
			case WM_DISPLAYCHANGE:  obs->OnDisplayChange(LOWORD(lParam), HIWORD(lParam)); break;
			//case WM_CLOSE:          obs->OnClose(); break;
			//case WM_DESTROY:        obs->OnDestroy(); break;

				// ── 렌더링 ──────────────────────────────────────────
			case WM_PAINT:          obs->OnPaint(); break;
			case WM_ERASEBKGND:     obs->OnEraseBackground(); break;
			}
		}
	}
}