#pragma once
#include <memory>
#include <filesystem>
#include <unordered_map>
#include <wrl/client.h>

//using namespace Microsoft::WRL::co

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

        //template<typename T>
        //std::shared_ptr<T> Load(const std::filesystem::path& path);
        //void UnLoadAll();

        //ID2D1Factory8* GetD2DFactory();
        //IDWriteFactory* GetDWriteFactory();
        //IWICImagingFactory* GetWICFactory();

    private :
        ResourceManager() = default;
        ~ResourceManager() = default;

    //private :
    //    ID2D1DeviceContext7* m_context = nullptr;
    //    ComPtr<ID2D1Factory8>      m_d2dFactory;
    //    ComPtr<IDWriteFactory>     m_dwriteFactory;
    //    ComPtr<IWICImagingFactory> m_wicFactory;

    //    std::unordered_map<std::wstring,
    //        std::shared_ptr<Resource>> m_cache;
	};
}