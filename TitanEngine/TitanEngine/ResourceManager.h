#pragma once
#include "Resource.h"
#include "DebugConsole/DebugConsole.h"
#include <memory>
#include <filesystem>
#include <unordered_map>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace TitanEngine
{

	class ResourceManager
	{
	public :
        static ResourceManager& Instance()
        {
            static ResourceManager instance;
            return instance;
        }
        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;

        bool Initialize(ID2D1DeviceContext7* ctx);

        template<typename T>
        static std::shared_ptr<T> Load(const std::filesystem::path& path)
        {
            return Instance().LoadImpl<T>(path);
        }

        void UnLoadAll();
    

        IDWriteFactory* GetDWriteFactory() { return m_dwriteFactory.Get(); }
        IWICImagingFactory* GetWICFactory() { return m_wicFactory.Get(); }

    private :
        ResourceManager() = default;
        ~ResourceManager() = default;
        template<typename T>
        std::shared_ptr<T> LoadImpl(const std::filesystem::path& path)
        {
            const std::wstring key = path.wstring();
            auto it = m_cache.find(key);
            if (it != m_cache.end())
                return std::static_pointer_cast<T>(it->second);

            auto res = std::make_shared<T>();
            if (!res->Load(m_context, path))
            {
                LOG_ERROR("Resource load failed: %ls", path.c_str());
                return nullptr;
            }
            m_cache[key] = res;
            return res;
        }

    private :
        ID2D1DeviceContext7*       m_context = nullptr;
        ComPtr<IDWriteFactory>     m_dwriteFactory;
        ComPtr<IWICImagingFactory> m_wicFactory;
        std::unordered_map<std::wstring,
            std::shared_ptr<Resource>> m_cache;
	};
}