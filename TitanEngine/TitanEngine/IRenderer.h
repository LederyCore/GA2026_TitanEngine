#pragma once

namespace TitanEngine
{
	class RenderSystem; // 전방선언
}

namespace TitanEngine::Renderer
{
	class IRenderer
	{
	public:
		virtual ~IRenderer() = default;

		virtual void ShutDown() = 0;
		virtual bool Initialize() = 0;
		virtual void RenderBegin() = 0;
		virtual void RenderEnd() = 0;
		virtual void ShowFPS(float fps) = 0;

		// RenderSystem을 받아서 내부에서 DeviceContext로 처리
		virtual void RenderScene(TitanEngine::RenderSystem* rs) = 0;

		virtual void DrawCircle(float x, float y, float radius, const D2D1::ColorF& color) = 0;
	};
}