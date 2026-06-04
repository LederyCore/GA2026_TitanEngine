#pragma once

class IRenderer
{
public :
	virtual ~IRenderer() = default;
	virtual bool Initialize() = 0;
	virtual void RenderBegin() = 0;
	virtual void RenderEnd() = 0;

	virtual void DrawCircle(float x, float y, float radius, const D2D1::ColorF& color) = 0;
};

