#pragma once
#include <unordered_map>
#include <vector>
#include "IWindow.h"
#include "IWindowPublisher.h"

namespace Platform
{
	class Win32Window : public IWindow, public IWindowPublisher
	{
	public :
		Win32Window() = default;
		virtual ~Win32Window() override;

		// IWindow을(를) 통해 상속됨
		virtual HWND Create(const std::wstring& windowName, int width, int height) override;
		void Show() override;
		void Close() override;
		HWND GetHWND() override { return m_hWnd; }
		virtual LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

		// IWindowPublisher을(를) 통해 상속됨
		void AddObserver(IWindowObserver* obs) override;
		void RemoveObserver(IWindowObserver* obs) override;
		void AddObserver(UINT msg, IWindowObserver* obs) override;
		void RemoveObserver(UINT msg, IWindowObserver* obs) override;
		
		// this를 GWLP_USERDATA에 등록하므로 복사/이동 금지
		Win32Window(const Win32Window&) = delete;
		Win32Window& operator=(const Win32Window&) = delete;
		Win32Window(Win32Window&&) = delete;
		Win32Window& operator=(Win32Window&&) = delete;

	private :
		static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		
		// IWindowPublisher을(를) 통해 상속됨
		void Notify(UINT msg, WPARAM wParam, LPARAM lParam) override;

	private :
		HWND m_hWnd = nullptr;
		int m_width = 0, m_height = 0;
		std::unordered_map<UINT, std::vector<IWindowObserver*>> m_observerMap;

	};
}