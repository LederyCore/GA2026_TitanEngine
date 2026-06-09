#include "pch.h"
#include "D2DRenderer.h"
#include "DebugConsole/DebugConsole.h"
#include "Win32Window/IWindowObserver.h"

TitanEngine::Renderer::D2DRenderer::~D2DRenderer()
{
	UnInitialize();
}

bool TitanEngine::Renderer::D2DRenderer::Initialize(HWND hwnd)
{
	m_hWnd = hwnd;

	// 초기 크기 가져오기
	RECT rect;
	GetClientRect(hwnd, &rect);
	m_width = rect.right - rect.left;
	m_height = rect.bottom - rect.top;

	CreateDeviceAndSwapChain(hwnd);
	CreateRenderTargets();

	ComPtr<IWICImagingFactory> wicFactory;

	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&wicFactory));

	DX::ThrowIfFailed(hr);

	// DirectWrite 팩토리 생성
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(m_dwriteFactory.GetAddressOf())
	);

	// FPS 텍스트 포맷 생성
	m_dwriteFactory->CreateTextFormat(
		L"Consolas",               // 폰트
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		20.0f,                     // 폰트 크기
		L"ko-KR",
		m_fpsTextFormat.GetAddressOf()
	);

	return true;
}

void TitanEngine::Renderer::D2DRenderer::UnInitialize()
{
	ReleaseRenderTargets();

	m_targetBitmap = nullptr;
	m_brush = nullptr;
	m_d2dContext = nullptr;
	m_d2dDevice = nullptr;
	m_swapChain = nullptr;
	m_d3dDevice = nullptr;
}

void TitanEngine::Renderer::D2DRenderer::Resize(UINT width, UINT height)
{
	m_width = width;   // ← 추가
	m_height = height;  // ← 추가

	ReleaseRenderTargets();

	DX::ThrowIfFailed(m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0));

	CreateRenderTargets();
}

void TitanEngine::Renderer::D2DRenderer::OnResize(int width, int height)
{
	Resize(width, height);
}

void TitanEngine::Renderer::D2DRenderer::ShowFPS(float fps)
{
	if (!m_d2dContext || !m_fpsTextFormat || !m_brush) return;

	// float → wstring 변환
	wchar_t buffer[64];
	swprintf_s(buffer, L"FPS: %.1f", fps);

	// 렌더링 위치 (좌측 상단)
	D2D1_RECT_F rect = D2D1::RectF(10.0f, 10.0f, 300.0f, 40.0f);

	// 브러시 색상 초록으로 임시 변경
	m_brush->SetColor(D2D1::ColorF(D2D1::ColorF::LimeGreen));

	m_d2dContext->DrawText(
		buffer,
		static_cast<UINT32>(wcslen(buffer)),
		m_fpsTextFormat.Get(),
		rect,
		m_brush.Get()
	);
}

void TitanEngine::Renderer::D2DRenderer::RenderBegin()
{
	m_d2dContext->BeginDraw();
	m_d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::White));
}

void TitanEngine::Renderer::D2DRenderer::RenderEnd()
{
	m_d2dContext->EndDraw();

	HRESULT hr = m_swapChain->Present(1, 0);

	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		UnInitialize();
		Initialize(m_hWnd);
	}
	else
	{
		DX::ThrowIfFailed(hr);
	}
}

void TitanEngine::Renderer::D2DRenderer::CreateDeviceAndSwapChain(HWND hwnd)
{
	//1. D3D11 디바이스 생성
	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
	ComPtr<ID3D11Device> d3dDevice;

	HRESULT hr = D3D11CreateDevice(
		nullptr,                            //[in, optional]  IDXGIAdapter* pAdapter
		D3D_DRIVER_TYPE_HARDWARE,           //D3D_DRIVER_TYPE DriverType
		nullptr,                            //HMODULE         Software
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,   //UINT            Flags
		featureLevels,                      //[in, optional]  const D3D_FEATURE_LEVEL* pFeatureLevels
		ARRAYSIZE(featureLevels),           //UINT            FeatureLevels
		D3D11_SDK_VERSION,                  //UINT            SDKVersion
		&d3dDevice,                         //[out, optional] ID3D11Device** ppDevice
		nullptr,                            //[out, optional] D3D_FEATURE_LEVEL* pFeatureLevel
		nullptr);                       //[out, optional] ID3D11DeviceContext** ppImmediateContext

	DX::ThrowIfFailed(hr);

	// 2. DXGI 스왑체인 생성
	ComPtr<IDXGIDevice> dxgiDevice;
	hr = d3dDevice.As(&dxgiDevice);

	DX::ThrowIfFailed(hr);

	ComPtr<IDXGIAdapter> dxgiAdapter;
	hr = dxgiDevice->GetAdapter(&dxgiAdapter);

	DX::ThrowIfFailed(hr);

	ComPtr<IDXGIFactory2> dxgiFactory;
	hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));

	DX::ThrowIfFailed(hr);

	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scDesc.SampleDesc.Count = 1;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.BufferCount = 2;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.Scaling = DXGI_SCALING_STRETCH;

	ComPtr<IDXGISwapChain1> swapChain;
	hr = dxgiFactory->CreateSwapChainForHwnd(
		d3dDevice.Get(), hwnd, &scDesc, nullptr, nullptr, &swapChain);

	DX::ThrowIfFailed(hr);

	// 3. ID2D1Factory4 생성
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

	DX::ThrowIfFailed(hr);

	// 4. ID2D1Device4 생성
	ComPtr<ID2D1Device> baseDevice;
	hr = d2dFactory->CreateDevice(dxgiDevice.Get(), &baseDevice);

	DX::ThrowIfFailed(hr);

	ComPtr<ID2D1Device7> d2dDevice;
	hr = baseDevice.As(&d2dDevice);

	DX::ThrowIfFailed(hr);

	// 5. ID2D1DeviceContext7 생성
	ComPtr<ID2D1DeviceContext7> d2dContext;//
	hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dContext);

	DX::ThrowIfFailed(hr);

	m_d3dDevice = d3dDevice;
	m_swapChain = swapChain;

	m_d2dDevice = d2dDevice;
	m_d2dContext = d2dContext;
}

void TitanEngine::Renderer::D2DRenderer::CreateRenderTargets()
{
	// 6. SwapChain 백버퍼 -> D2D Bitmap1 을 생성하여 렌더 타겟으로 설정

	ComPtr<IDXGISurface> dxgiSurface;
	HRESULT hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiSurface));

	DX::ThrowIfFailed(hr);

	D2D1_BITMAP_PROPERTIES1 bitmapProps = {};
	bitmapProps.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
	bitmapProps.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	bitmapProps.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	bitmapProps.dpiX = bitmapProps.dpiY = 96.0f;

	ComPtr<ID2D1Bitmap1> targetBitmap;
	hr = m_d2dContext->CreateBitmapFromDxgiSurface(dxgiSurface.Get(), &bitmapProps, targetBitmap.GetAddressOf());

	DX::ThrowIfFailed(hr);

	// 렌더 타겟 설정
	m_d2dContext->SetTarget(targetBitmap.Get());

	m_targetBitmap = targetBitmap;

	hr = m_d2dContext->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Blue),
		&m_brush);

	DX::ThrowIfFailed(hr);
}

void TitanEngine::Renderer::D2DRenderer::ReleaseRenderTargets()
{
	if (m_d2dContext)
	{
		m_d2dContext->SetTarget(nullptr);
	}

	m_targetBitmap.Reset();
	m_brush.Reset();
}
