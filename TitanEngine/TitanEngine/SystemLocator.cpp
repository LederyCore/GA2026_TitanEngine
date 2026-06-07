#include "pch.h"
#include "SystemLocator.h"

namespace TitanEngine
{
    UpdateSystem* SystemLocator::s_updateSystem = nullptr;
    RenderSystem* SystemLocator::s_renderSystem = nullptr;
}