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
		

	private :
		static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		
		// IWindowPublisher을(를) 통해 상속됨
		void Notify(UINT msg, WPARAM wParam, LPARAM lParam) override;

	private :
		int m_width, m_height;
		HWND m_hWnd = nullptr;
		std::unordered_map<UINT, std::vector<IWindowObserver*>> m_observerMap;

	};
}