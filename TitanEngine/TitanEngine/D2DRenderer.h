#pragma once

#include  "IRenderer.h"
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
	void RenderBegin() override;
	void RenderEnd() override;

	void DrawCircle(float x, float y, float radius, const D2D1::ColorF& color) override;

	int m_screenWidth; // 렌더링할 최종 화면의 해상도
	int m_screenHeight;
	HWND m_mainWindow;

	//D3D member
	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;
	ComPtr<IDXGISwapChain> m_swapChain;

	//D2D member
	ComPtr<ID2D1Device7> m_d2dDevice;
	ComPtr<ID2D1DeviceContext7> m_d2dContext;
	ComPtr<ID2D1Bitmap1> m_targetBitmap;
	ComPtr<ID2D1SolidColorBrush> m_brush;
	ComPtr<ID2D1Factory4> m_d2dFactory;

public:
	void Present();


private:
	bool CreateDeviceAndSwapChain();
	bool CreateRenderTargets();
};

