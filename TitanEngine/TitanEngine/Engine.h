#pragma once

namespace Platform
{
	class IWindow;
}


namespace TitanEngine
{
	namespace Time
	{
		class GameTimer;
	}

	namespace SceneManagement
	{
		class SceneGraph;
		class Scene;
	}

	namespace Renderer
	{
		class IRenderer;
	}
	class Engine
	{
	public:
		Engine();
		virtual ~Engine();

		bool Initialize(Platform::IWindow& window, const wchar_t* windowName, int width, int height);
		void Run();
		void Finalize();

	private:
		void FixedUpdate(float fixedTime);
		void Update(float deltaTime);
		void LateUpdate(float deltaTime);
		void Render();

	private:
		Time::GameTimer* m_timer = nullptr;
		Platform::IWindow* m_window = nullptr;
		Renderer::IRenderer* m_renderer = nullptr;

		SceneManagement::Scene* m_currentFrameActiveScene = nullptr;

		float m_fDeltaTime = 0;
		float m_fFrameCount = 0;

	public:
		ComPtr<ID2D1Bitmap1> m_bitmapCat;

	};


}