#pragma once
#include <filesystem>

namespace TitanEngine
{
    class Resource
    {
    public:
        virtual ~Resource() = default;
        virtual bool Load(ID2D1DeviceContext7* ctx, const std::filesystem::path& path) = 0;
    };
}
