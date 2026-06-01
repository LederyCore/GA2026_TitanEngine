#include "pch.h"
#include "Win32Window.h"

Platform::Win32Window::~Win32Window()
{
	Close();
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

void Platform::Win32Window::SetHWND(HWND hwnd)
{
	m_hWnd = hwnd;
}

LRESULT Platform::Win32Window::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

}