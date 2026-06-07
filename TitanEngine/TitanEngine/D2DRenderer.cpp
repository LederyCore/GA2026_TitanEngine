#include "pch.h"
#include "D2DRenderer.h"
#include "RenderSystem.h"
#include <iostream>

namespace TitanEngine::Renderer
{
    bool D2DRenderer::Initialize()
    {
        if (!CreateDeviceAndSwapChain())  return false;
        if (!CreateRenderTargets())       return false;
        if (!CreateDWriteResources())     return false;
        if (!CreateWICFactory())          return false;  // 추가
        return true;
    }

    void D2DRenderer::ShutDown()
    {
        if (m_d2dContext)
        {
            m_d2dContext->Flush();          // 1. 대기 중인 GPU 명령 비우기
            m_d2dContext->SetTarget(nullptr); // 2. 렌더 타겟 해제
        }

        // 3. D2D 리소스 해제 (생성 역순)
        m_brush.Reset();
        m_targetBitmap.Reset();
        m_textFormat.Reset();
        m_dwriteFactory.Reset();

        // 4. Context → Device → Factory 순
        m_d2dContext.Reset();
        m_d2dDevice.Reset();
        m_d2dFactory.Reset();

        // 5. DXGI/D3D 마지막
        m_swapChain.Reset();
        m_context.Reset();
        m_device.Reset();
    }


    bool D2DRenderer::CreateDeviceAndSwapChain()
    {
        // 1. D3D11 디바이스 생성
        {
            ComPtr<ID3D11Device>        device;
            ComPtr<ID3D11DeviceContext> context;

            D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
            D3D_FEATURE_LEVEL d3dFeatureLevel;

            HRESULT hr = D3D11CreateDevice(
                nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                featureLevels, ARRAYSIZE(featureLevels),
                D3D11_SDK_VERSION,
                &device, &d3dFeatureLevel, &context);
            if (FAILED(hr)) return false;

            if (FAILED(device.As(&m_device)))   return false;
            if (FAILED(context.As(&m_context))) return false;
        }

        // 2. DXGI 스왑체인 생성
        ComPtr<IDXGIDevice> dxgiDevice;
        m_device.As(&dxgiDevice);

        {
            ComPtr<IDXGIAdapter> dxgiAdapter;
            dxgiDevice->GetAdapter(&dxgiAdapter);

            ComPtr<IDXGIFactory2> dxgiFactory;
            dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));

            UINT numQualityLevels = 0;
            m_device->CheckMultisampleQualityLevels(
                DXGI_FORMAT_R8G8B8A8_UNORM, 4, &numQualityLevels);

            DXGI_SWAP_CHAIN_DESC sd = {};
            sd.BufferDesc.Width = m_screenWidth;
            sd.BufferDesc.Height = m_screenHeight;
            sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            sd.BufferDesc.RefreshRate.Numerator = 60;
            sd.BufferDesc.RefreshRate.Denominator = 1;
            sd.BufferCount = 2;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.OutputWindow = m_mainWindow;
            sd.Windowed = TRUE;
            sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
            sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

            if (numQualityLevels > 0) {
                sd.SampleDesc.Count = 4;
                sd.SampleDesc.Quality = numQualityLevels - 1;
            }
            else {
                sd.SampleDesc.Count = 1;
                sd.SampleDesc.Quality = 0;
            }

            ComPtr<IDXGISwapChain> swapChain;
            HRESULT hr = dxgiFactory->CreateSwapChain(m_device.Get(), &sd, &swapChain);
            if (FAILED(hr)) return false;
            swapChain.As(&m_swapChain);
        }

        // 3. D2D Factory → Device → Context (스코프로 중간 참조 정리)
        {
            D2D1_FACTORY_OPTIONS opts = {};
#if defined(_DEBUG)
            opts.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
            ComPtr<ID2D1Factory8> d2dFactory;
            HRESULT hr = D2D1CreateFactory(
                D2D1_FACTORY_TYPE_SINGLE_THREADED,
                __uuidof(ID2D1Factory8), &opts,
                reinterpret_cast<void**>(d2dFactory.GetAddressOf()));
            if (FAILED(hr)) return false;
            if (FAILED(d2dFactory.As(&m_d2dFactory))) return false;
        }

        {
            ComPtr<ID2D1Device> baseDevice;
            HRESULT hr = m_d2dFactory->CreateDevice(dxgiDevice.Get(), &baseDevice);
            if (FAILED(hr)) return false;
            if (FAILED(baseDevice.As(&m_d2dDevice))) return false;
        }

        {
            ComPtr<ID2D1DeviceContext> baseContext;
            HRESULT hr = m_d2dDevice->CreateDeviceContext(
                D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &baseContext);
            if (FAILED(hr)) return false;
            if (FAILED(baseContext.As(&m_d2dContext))) return false;
        }

        return true;
    }

    bool D2DRenderer::CreateDWriteResources()
    {
        HRESULT hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(m_dwriteFactory.GetAddressOf())
        );
        if (FAILED(hr)) return false;

        hr = m_dwriteFactory->CreateTextFormat(
            L"굴림",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            20.0f,
            L"ko-kr",
            m_textFormat.GetAddressOf()
        );
        return SUCCEEDED(hr);
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
        if (FAILED(hr)) return false;
        m_brush = brush;
        // ============================================================================================

        return true;
    }

    bool D2DRenderer::CreateWICFactory()
    {
        HRESULT hr = CoCreateInstance(
            CLSID_WICImagingFactory,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&m_wicFactory));
        return SUCCEEDED(hr);
    }

    void D2DRenderer::ShowFPS(float fps)
    {
#ifdef _DEBUG
        if (!m_d2dContext || !m_brush || !m_textFormat)
            return;

        wchar_t buffer[64];
        swprintf_s(buffer, L"FPS : %.0f", fps);

        m_brush->SetColor(D2D1::ColorF(D2D1::ColorF::LimeGreen));

        m_d2dContext->DrawText(
            buffer,
            static_cast<UINT32>(wcslen(buffer)),
            m_textFormat.Get(),
            D2D1::RectF(2.5f, 2.5f, 300.0f, 40.0f),  // 출력 영역
            m_brush.Get()
        );
#endif
    }


    void D2DRenderer::RenderBegin()
    {
        if (!m_d2dContext)
            return;

        m_d2dContext->BeginDraw();
        m_d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::White)); // 배경을 흰색으로 초기화
    }

    void D2DRenderer::RenderScene(TitanEngine::RenderSystem* rs)
    {
        if (!rs || !m_d2dContext) return;
        rs->Render(m_d2dContext.Get());
    }

    void D2DRenderer::RenderEnd()
    {
        if (!m_d2dContext)
            return;

        HRESULT hr = m_d2dContext->EndDraw();
        if (FAILED(hr))
            return;

        Present();
    }

    void D2DRenderer::DrawCircle(float x, float y, float radius, const D2D1::ColorF& color)
    {
        if (!m_d2dContext || !m_brush || radius <= 0.0f)
            return;

        m_brush->SetColor(color);
        m_d2dContext->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius), m_brush.Get());
    }

    // 렌더링 작업이 끝나면 스왑체인에 프레임을 표시
    void D2DRenderer::Present()
    {
        if (!m_swapChain)
            return;

        // 백 퍼버 ↔ 프론트 버퍼 스와핑
        HRESULT hr = m_swapChain->Present(1, 0);

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            ShutDown();     // 디바이스가 제거되거나 리셋된 경우, 재초기화 필요
            if (!Initialize())  // ← 재초기화 실패 시 처리
            {
                std::cerr << "Failed to reinitialize after device loss." << std::endl;
            }
        }
        else if (FAILED(hr))
        {
            std::cerr << "Present failed: " << std::hex << hr << std::endl;
        }
    }

    //void D2DRenderer::CreateBitmapFromFile(const wchar_t* path, ID2D1Bitmap1*& outBitmap)
    //{
    //    ComPtr<IWICBitmapDecoder>     decoder;
    //    ComPtr<IWICBitmapFrameDecode> frame;
    //    ComPtr<IWICFormatConverter>   converter;
    //
    //
    //    HRESULT hr = m_wicFactory->CreateDecoderFromFilename(
    //        path, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
    //
    //    if (FAILED(hr))
    //        return;
    //
    //
    //    hr = decoder->GetFrame(0, &frame);
    //
    //    if (FAILED(hr))
    //        return;
    //
    //    hr = m_wicFactory->CreateFormatConverter(&converter);
    //
    //    DX::ThrowIfFailed(hr);
    //
    //
    //    hr = converter->Initialize(
    //        frame.Get(),
    //        GUID_WICPixelFormat32bppPBGRA,
    //        WICBitmapDitherTypeNone,
    //        nullptr,
    //        0.0f,
    //        WICBitmapPaletteTypeCustom
    //    );
    //
    //    DX::ThrowIfFailed(hr);
    //
    //    // Direct2D 비트맵 속성 (premultiplied alpha, B8G8R8A8_UNORM)
    //    D2D1_BITMAP_PROPERTIES1 bmpProps = D2D1::BitmapProperties1(
    //        D2D1_BITMAP_OPTIONS_NONE,
    //        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
    //    );
    //
    //    // ⑥ DeviceContext에서 WIC 비트맵으로부터 D2D1Bitmap1 생성
    //    hr = m_d2dContext->CreateBitmapFromWicBitmap(converter.Get(), &bmpProps, &outBitmap);
    //}

}