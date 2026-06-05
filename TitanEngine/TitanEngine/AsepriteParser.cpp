#include "pch.h"
#include <fstream>
#include "json.hpp"
#include "AsepriteParser.h"

AnimationClips AsepriteParser::Load(const std::filesystem::path& jsonPath)
{
    // 1) JSON 로드
    std::ifstream in{ jsonPath };
    if (!in) throw std::runtime_error("Cannot open " + jsonPath.string());
    nlohmann::json doc = nlohmann::json::parse(in);

    // 2) 전체 프레임 벡터 생성
    std::vector<Frame> allFrames;
    allFrames.reserve(doc["frames"].size());

    for (auto& f : doc["frames"])
    {
        auto& r = f["frame"];

        allFrames.push_back(Frame{
            D2D1_RECT_U{
                r["x"].get<UINT>(),
                r["y"].get<UINT>(),
                static_cast<UINT>(r["x"].get<UINT>() + r["w"].get<UINT>()),
                static_cast<UINT>(r["y"].get<UINT>() + r["h"].get<UINT>())
            },
            f["duration"].get<float>() / 1000.f
            });
    }

    // 3) 태그별 clip 생성
    AnimationClips clips;
    for (auto& tag : doc["meta"]["frameTags"])
    {
        std::string name = tag["name"];
        int from = tag["from"], to = tag["to"];

        AnimationClip clip;
        for (int i = from; i <= to; ++i)
            clip.AddFrame(allFrames[i]);

        clips.emplace_back(std::move(name), std::move(clip));
    }

    return clips;
}
