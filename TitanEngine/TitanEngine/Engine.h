#pragma once

namespace Platform
{
	class IWindow;
}

namespace TitanEngine
{
	class Engine
	{
	public :
		Engine() = default;
		virtual ~Engine();

		bool Initialize(Platform::IWindow& window, const wchar_t* windowName, int width, int height);
		void Run();
		void Finalize();

	private :
		void Update();
		void Render();

	private :
		Platform::IWindow* m_window = nullptr;

	};
}