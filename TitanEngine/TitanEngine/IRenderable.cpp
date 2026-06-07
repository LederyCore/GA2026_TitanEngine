#include "pch.h"
#include "IRenderable.h"
#include "RenderSystem.h"

TitanEngine::IRenderable::~IRenderable()
{
    // 소멸 시 자동 해제
    auto* rs = SystemLocator::GetRenderSystem();
    if (rs) rs->Unregister(this);
}
