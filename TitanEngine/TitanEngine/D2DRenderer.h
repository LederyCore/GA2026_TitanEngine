#pragma once
#include  "IRenderer.h"

namespace TitanEngine::Renderer
{
	class D2DRenderer : public IRenderer
	{
	public:
		D2DRenderer(int width, int height, HWND window)
		{
			m_screenWidth = width;
			m_screenHeight = height;
			m_mainWindow = window;
		}

		bool Initialize() override;
		void ShutDown() override;
		void RenderBegin() override;
		void Present();
		void RenderEnd() override;
		void ShowFPS(float fps) override;


		void DrawBitmap(ID2D1Bitmap1* bitmap, D2D1_RECT_F dest) override;
		void RenderScene(TitanEngine::RenderSystem* rs) override;
		// 테스트
		void DrawCircle(float x, float y, float radius, const D2D1::ColorF& color) override;

	private:
		bool CreateDeviceAndSwapChain();
		bool CreateDWriteResources();
		bool CreateRenderTargets();
		bool CreateWICFactory();

	public:
		int m_screenWidth; // 렌더링할 최종 화면의 해상도
		int m_screenHeight;
		HWND m_mainWindow;

	private:
		//D3D member
		ComPtr<ID3D11Device> m_device;
		ComPtr<ID3D11DeviceContext> m_context;
		ComPtr<IDXGISwapChain> m_swapChain;

	public:
		void CreateBitmapFromFile(const wchar_t* path, ID2D1Bitmap1*& outBitmap) override;
		//D2D member
		ComPtr<ID2D1Device7> m_d2dDevice;
		ComPtr<ID2D1DeviceContext7> m_d2dContext;
		ComPtr<ID2D1Bitmap1> m_targetBitmap;
		ComPtr<ID2D1SolidColorBrush> m_brush;
		ComPtr<ID2D1Factory4> m_d2dFactory;
		ComPtr<IDWriteFactory>     m_dwriteFactory;
		ComPtr<IDWriteTextFormat>  m_textFormat;
		ComPtr<IWICImagingFactory> m_wicFactory;
	};

}