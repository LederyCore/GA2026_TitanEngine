#include "pch.h"
//#include "AnimationClip.h"
//#include "AsepriteParser.h"
//#include "D2DRenderer.h"
//#include "AssetManager.h"
//
//// Load or get cached texture
//ID2D1Bitmap* AssetManager::LoadTexture(D2DRender* pRenderer, const std::wstring& key, const std::filesystem::path& filepath)
//{
//    auto it = m_textures.find(key);
//
//    if (it != m_textures.end()) return it->second.Get();
//
//    pRenderer->CreateBitmapFromFile(filepath.c_str(), *m_textures[key].GetAddressOf());
//
//    return m_textures[key].Get();
//}
//
//
//const AnimationClips& AssetManager::LoadAseprite(D2DRender* pRenderer, const std::wstring& key,
//                                                 const std::filesystem::path& jsonFile)
//{
//    auto it = m_clipsMap.find(key);
//    if (it != m_clipsMap.end())
//        return it->second;
//
//
//    // 1) Load texture
//    Microsoft::WRL::ComPtr<ID2D1Bitmap1> sheet = GetTexture(key);
//    std::wstring keyW = key + L".png";
//
//    pRenderer->CreateBitmapFromFile(keyW.c_str(), *(sheet.GetAddressOf()));
//
//
//    auto tempMap = AsepriteParser::Load(jsonFile);
//
//
//    AnimationClips clips;
//    clips.reserve(tempMap.size());
//    for (auto& [name, clip] : tempMap)
//    {
//        clip.SetBitmap(sheet.Get());
//        clips.emplace_back(std::move(name), std::move(clip));
//    }
//
//    auto res = m_clipsMap.emplace(key, std::move(clips));
//
//    return res.first->second;
//}
