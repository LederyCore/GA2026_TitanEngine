#pragma once
#include <string>

namespace Platform
{
	class IWindow
	{
	public :
		virtual ~IWindow() = default;
		virtual HWND Create(const std::wstring& windowName, int width, int height) = 0;
		virtual void Show() = 0;
		virtual void Close() = 0;
		virtual HWND GetHWND() = 0;
		virtual LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;
	};
}