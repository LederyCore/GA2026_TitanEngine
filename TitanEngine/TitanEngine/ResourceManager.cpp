#include "pch.h"
#include "ResourceManager.h"

namespace TitanEngine
{
	bool ResourceManager::Initialize(ID2D1DeviceContext7* ctx)
	{
		m_context = ctx;
		if (!m_context) return false;
		return true;
	}


}