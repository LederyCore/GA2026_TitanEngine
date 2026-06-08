#pragma once

namespace TitanEngine { class RenderSystem; }

class IRenderer
{
public:
    virtual ~IRenderer() = default;


    virtual bool Initialize() = 0;
    virtual void ShutDown() = 0;
    virtual void RenderBegin() = 0;
    virtual void RenderEnd() = 0;
    virtual void ShowFPS(float fps) = 0;
    virtual void RenderScene(TitanEngine::RenderSystem* rs) = 0;
    virtual void DrawCircle(float x, float y, float radius,
        const D2D1::ColorF& color) = 0;

	virtual void CreateBitmapFromFile(const wchar_t* path, ID2D1Bitmap1*& outBitmap) = 0;
	virtual void DrawBitmap(ID2D1Bitmap1* bitmap, D2D1_RECT_F dest) = 0;
};
