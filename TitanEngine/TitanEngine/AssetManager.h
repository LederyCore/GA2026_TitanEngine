#pragma once

#include <unordered_map>
#include <filesystem>
#include <wrl/client.h>
#include "AnimationClip.h"

class D2DRenderer;

class AssetManager
{
public:
    using AnimationClips = std::vector<std::pair<std::string, AnimationClip>>;
    using D2DRender = D2DRenderer;

    AssetManager() = default;
    ~AssetManager() = default;

    ID2D1Bitmap* LoadTexture(D2DRenderer* pRenderer, const std::wstring& key, const std::filesystem::path& filepath);

    const AnimationClips& LoadAseprite(D2DRenderer* pRenderer, const std::wstring& key,
                                       const std::filesystem::path& jsonFile);

    // Retrieve clips
    const AnimationClips& GetClips(const std::wstring& key) const
    {
        static const AnimationClips emptyClips;

        auto it = m_clipsMap.find(key);

        if (it == m_clipsMap.end())
            return emptyClips;

        return it->second;
    }

    // Retrieve texture
    ID2D1Bitmap1* GetTexture(const std::wstring& key) const
    {
        auto it = m_textures.find(key);
        if (it == m_textures.end()) return nullptr;

        return it->second.Get();
    }

    // Clear all assets
    void Clear()
    {
        m_textures.clear();
        m_clipsMap.clear();
    }

private:

    std::unordered_map<std::wstring, ComPtr<ID2D1Bitmap1>> m_textures;
    std::unordered_map<std::wstring, AnimationClips> m_clipsMap;
};

