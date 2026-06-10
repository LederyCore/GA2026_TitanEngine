#pragma once
#include <wrl/client.h>
#include <d2d1helper.h>
#include "Win32Window/IWindowObserver.h"
using namespace Microsoft::WRL;

namespace TitanEngine::Renderer
{
    class D2DRenderer final : public Platform::IWindowObserver
    {
    public:
        D2DRenderer() = default;
        ~D2DRenderer();

        bool Initialize(HWND hwnd);
        void UnInitialize();
        void Resize(UINT width, UINT height);
        void OnResize(int width, int height) override;
        void OnExitSizeMove() override;
        void ShowFPS(float fps);

        void RenderBegin();
        void RenderEnd();

        // 외부에 제공하는 것
        ID2D1DeviceContext7*  GetContext() { return m_d2dContext.Get(); }
        ID2D1Device7*         GetDevice() { return m_d2dDevice.Get(); }  // 브러시 생성용
        ID2D1SolidColorBrush* GetBrush() { return m_brush.Get(); }       // 공용 브러시
        UINT GetWidth()  const { return m_width; }   // ← 추가
        UINT GetHeight() const { return m_height; }  // ← 추가


    private:
        void CreateDeviceAndSwapChain(HWND hwnd);
        void CreateRenderTargets();
        void ReleaseRenderTargets();

    private:
        HWND m_hWnd = nullptr;
        UINT m_width = 1280; 
        UINT m_height = 720; 

        bool m_isDrawing = false;
        bool m_needsResize = false;
        UINT m_pendingWidth = 0;
        UINT m_pendingHeight = 0;

        // D3D 인프라
        ComPtr<ID3D11Device>        m_d3dDevice;
        ComPtr<IDXGISwapChain1>     m_swapChain;

        // D2D 인프라
        ComPtr<ID2D1Device7>        m_d2dDevice;
        ComPtr<ID2D1DeviceContext7> m_d2dContext;
        ComPtr<ID2D1Bitmap1>        m_targetBitmap;

        // 공용 리소스
        ComPtr<ID2D1SolidColorBrush> m_brush;

        ComPtr<IDWriteFactory>       m_dwriteFactory;   
        ComPtr<IDWriteTextFormat>    m_fpsTextFormat;   
    };
}