#pragma once
#include <string>

namespace Platform
{
	class IWindow
	{
	public :
		virtual ~IWindow() = default;
		virtual void Show() = 0;
		virtual void Close() = 0;
		virtual void* GetHandle() = 0;
		virtual void SetHWND(HWND hwnd) = 0;
		virtual LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;
	};
}