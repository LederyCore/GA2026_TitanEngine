#include "pch.h"
#include "D2DRenderer.h"

#include <iostream>
#include <ostream>
bool D2DRenderer::Initialize()
{
 
    if (!CreateDeviceAndSwapChain())
        return false;

    if (!CreateRenderTargets())
        return false;
 
    return true;
}

void D2DRenderer::ShutDown()
{
    m_d2dContext->SetTarget(nullptr);  // ← 반드시 먼저 호출
    m_targetBitmap.Reset();
    m_brush.Reset();
    m_d2dContext.Reset();
    m_d2dDevice.Reset();
    m_d2dFactory.Reset();
    m_swapChain.Reset();
    m_context.Reset();
    m_device.Reset();
}


bool D2DRenderer::CreateDeviceAndSwapChain()
{
    //1. D3D11 디바이스 생성
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;

    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    D3D_FEATURE_LEVEL d3dFeatureLevel;

    HRESULT hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        featureLevels,
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        &device,
        &d3dFeatureLevel,
        &context);

    if (FAILED(hr))
        return false;

    UINT numQualityLevels;
    device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &numQualityLevels);
    if (numQualityLevels <= 0) {
        std::cout << "MSAA not supported." << std::endl;
    }

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferDesc.Width = m_screenWidth;               // set the back buffer width
    sd.BufferDesc.Height = m_screenHeight;             // set the back buffer height
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // use 32-bit color (D2D requires BGRA)
    sd.BufferCount = 2;                                // Double-buffering
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // how swap chain is to be used
    sd.OutputWindow = m_mainWindow;                    // the window to be used
    sd.Windowed = TRUE;                                // windowed/full-screen mode
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow full-screen switching
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    if (numQualityLevels > 0) {
        sd.SampleDesc.Count = 4; // how many multisamples
        sd.SampleDesc.Quality = numQualityLevels - 1;
    }
    else {
        sd.SampleDesc.Count = 1; // how many multisamples
        sd.SampleDesc.Quality = 0;
    }

    if (FAILED(device.As(&m_device))) {
        std::cout << "device.AS() failed." << std::endl;
        return false;
    }

    if (FAILED(context.As(&m_context))) {
        std::cout << "context.As() failed." << std::endl;
        return false;
    }

    // IDXGIFactory를 이용한 CreateSwapChain()
    // 2. DXGI 스왑체인 생성
    ComPtr<IDXGIDevice> dxgiDevice;
    m_device.As(&dxgiDevice);

    ComPtr<IDXGIAdapter> dxgiAdapter;
    dxgiDevice->GetAdapter(&dxgiAdapter);

    ComPtr<IDXGIFactory2> dxgiFactory;
    dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));

    ComPtr<IDXGISwapChain> swapChain;
    dxgiFactory->CreateSwapChain(m_device.Get(), &sd, &swapChain);

    swapChain.As(&m_swapChain);

    // 3. ID2D1Factory8 생성 ============================================================================================
    D2D1_FACTORY_OPTIONS opts = {};
    ComPtr<ID2D1Factory8> d2dFactory;

#if defined(_DEBUG)
    opts.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

    hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        __uuidof(ID2D1Factory8),
        &opts,
        reinterpret_cast<void**>(d2dFactory.GetAddressOf()));

    // 4. ID2D1Device7 생성 ============================================================================================
    ComPtr<ID2D1Device> baseDevice;
    hr = d2dFactory->CreateDevice(dxgiDevice.Get(), &baseDevice);
    if (FAILED(hr)) return false;
    m_d2dFactory = d2dFactory;

    ComPtr<ID2D1Device7> d2dDevice;
    hr = baseDevice.As(&d2dDevice);
    if (FAILED(hr)) return false;
    m_d2dDevice = d2dDevice;
    // ============================================================================================

    // 5. ID2D1DeviceContext7 생성 ============================================================================================
    ComPtr<ID2D1DeviceContext7> d2dContext;
    hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dContext);
    if (FAILED(hr)) return false;
    m_d2dContext = d2dContext;
    // ============================================================================================

}

bool D2DRenderer::CreateRenderTargets()
{
    // 6. SwapChain 백버퍼 -> D2D Bitmap1 을 생성하여 렌더 타겟으로 설정 ==============================================
    ComPtr<IDXGISurface> dxgiSurface;
    HRESULT hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiSurface));
    if (FAILED(hr)) return false;

    D2D1_BITMAP_PROPERTIES1 bitmapProps = {};
    bitmapProps.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    bitmapProps.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bitmapProps.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    bitmapProps.dpiX = bitmapProps.dpiY = 96.0f;

    ComPtr<ID2D1Bitmap1> targetBitmap;
    hr = m_d2dContext->CreateBitmapFromDxgiSurface(dxgiSurface.Get(), &bitmapProps, targetBitmap.GetAddressOf());
    if (FAILED(hr)) return false;
    m_d2dContext->SetTarget(targetBitmap.Get());
    m_targetBitmap = targetBitmap;
    // ============================================================================================

    // 7. 기본 브러시 생성 ============================================================================================
    ComPtr<ID2D1SolidColorBrush> brush;
    hr = m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Tomato), &brush);
    m_brush = brush;
    // ============================================================================================
}

void D2DRenderer::RenderBegin()
{
    m_d2dContext->BeginDraw();
    m_d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::White)); // 배경을 흰색으로 초기화
}

void D2DRenderer::RenderEnd()
{
    m_d2dContext->EndDraw();

    Present();
}

void D2DRenderer::DrawCircle(float x, float y, float radius, const D2D1::ColorF& color)
{
    m_brush->SetColor(color);
    m_d2dContext->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius), m_brush.Get());
}

// 렌더링 작업이 끝나면 스왑체인에 프레임을 표시
void D2DRenderer::Present()
{
    // 백 퍼버 ↔ 프론트 버퍼 스와핑
    HRESULT hr = m_swapChain->Present(1, 0);

    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        ShutDown();     // 디바이스가 제거되거나 리셋된 경우, 재초기화 필요
        Initialize();
    }
    else
    {
        if (FAILED(hr)) return;
    }
}



