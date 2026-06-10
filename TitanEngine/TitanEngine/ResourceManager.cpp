#include "pch.h"
#include "ResourceManager.h"

#pragma comment(lib, "windowscodecs.lib")

namespace TitanEngine
{
	bool ResourceManager::Initialize(ID2D1DeviceContext7* ctx)
	{
		m_context = ctx;
		if (!m_context) return false;

		HRESULT hr = CoCreateInstance(
			CLSID_WICImagingFactory, nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&m_wicFactory));
		if (FAILED(hr))
		{
			LOG_ERROR("WIC factory creation failed (HRESULT: 0x%08X)", hr);
			return false;
		}

		return true;
	}

	void ResourceManager::UnLoadAll()
	{
		m_cache.clear();
		m_wicFactory = nullptr;
		m_context = nullptr;
	}
}