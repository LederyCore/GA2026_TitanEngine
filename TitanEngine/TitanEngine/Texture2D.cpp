#include "pch.h"
#include "Texture2D.h"
#include "ResourceManager.h"

namespace TitanEngine
{
    bool Texture2D::Load(ID2D1DeviceContext7* ctx, const std::filesystem::path& path)
    {
        IWICImagingFactory* wicFactory = ResourceManager::Instance().GetWICFactory();
        if (!wicFactory || !ctx)
            return false;

        // Decode image file
        ComPtr<IWICBitmapDecoder> decoder;
        HRESULT hr = wicFactory->CreateDecoderFromFilename(
            path.wstring().c_str(), nullptr,
            GENERIC_READ, WICDecodeMetadataCacheOnLoad,
            &decoder);
        if (FAILED(hr)) return false;

        ComPtr<IWICBitmapFrameDecode> frame;
        hr = decoder->GetFrame(0, &frame);
        if (FAILED(hr)) return false;

        // Convert to 32bpp pBGRA (premultiplied, D2D native format)
        ComPtr<IWICFormatConverter> converter;
        hr = wicFactory->CreateFormatConverter(&converter);
        if (FAILED(hr)) return false;

        hr = converter->Initialize(
            frame.Get(), GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone, nullptr,
            0.f, WICBitmapPaletteTypeMedianCut);
        if (FAILED(hr)) return false;

        // Cache decoded pixels in a CPU-side WIC bitmap so we can lock them
        ComPtr<IWICBitmap> wicBitmap;
        hr = wicFactory->CreateBitmapFromSource(
            converter.Get(), WICBitmapCacheOnLoad, &wicBitmap);
        if (FAILED(hr)) return false;

        UINT width = 0, height = 0;
        hr = wicBitmap->GetSize(&width, &height);
        if (FAILED(hr) || width == 0 || height == 0) return false;

        // Lock pixels for reading
        WICRect lockRect = { 0, 0, (INT)width, (INT)height };
        ComPtr<IWICBitmapLock> lock;
        hr = wicBitmap->Lock(&lockRect, WICBitmapLockRead, &lock);
        if (FAILED(hr)) return false;

        UINT stride = 0;
        hr = lock->GetStride(&stride);
        if (FAILED(hr)) return false;

        UINT bufferSize = 0;
        BYTE* pixels = nullptr;
        hr = lock->GetDataPointer(&bufferSize, &pixels);
        if (FAILED(hr) || !pixels) return false;

        // Upload pixels directly to a D2D bitmap.
        // Using CreateBitmap (not CreateBitmapFromWicBitmap) avoids an internal
        // WIC back-reference inside d2d1.dll that causes an AV on Release.
        D2D1_BITMAP_PROPERTIES1 props = {};
        props.pixelFormat.format    = DXGI_FORMAT_B8G8R8A8_UNORM;
        props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
        props.bitmapOptions         = D2D1_BITMAP_OPTIONS_NONE;
        props.dpiX = props.dpiY     = 96.0f;

        D2D1_SIZE_U size = { width, height };
        ComPtr<ID2D1Bitmap1> bmp;
        hr = ctx->CreateBitmap(size, pixels, stride, &props, &bmp);

        // Unlock before checking result
        lock.Reset();

        if (FAILED(hr)) return false;

        m_width  = width;
        m_height = height;
        m_bitmap = std::move(bmp);

        return true;
    }
}
