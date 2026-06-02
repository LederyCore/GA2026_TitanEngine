#include "pch.h"
#include "Engine.h"
#include "IWindow.h"

using namespace Platform;

TitanEngine::Engine::~Engine()
{

}

bool TitanEngine::Engine::Initialize(IWindow& window, const wchar_t* windowName, int width, int height)
{
	m_window = &window;
	void* handle = m_window->Create(windowName, width, height);
	
	if (!handle) return false;




	return true;
}

void TitanEngine::Engine::Run()
{
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{

		}
	}

}

void TitanEngine::Engine::Finalize()
{

}