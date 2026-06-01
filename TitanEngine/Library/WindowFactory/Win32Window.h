#pragma once
#include "IWindow.h"
#include "Multicastdelegate.h"

namespace Platform
{
	class Win32Window : public IWindow
	{
	public :
		Win32Window(const HWND& m_hWnd) : m_hWnd(m_hWnd) { }
		virtual ~Win32Window() override;

		// IWindow을(를) 통해 상속됨
		void Show() override;
		void Close() override;
		void* GetHandle() override { return m_hWnd; }
		void SetHWND(HWND hwnd) override;
		LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

	private :
		HWND m_hWnd = nullptr;



	};
}