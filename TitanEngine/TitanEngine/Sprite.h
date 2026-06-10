#pragma once
#include <memory>
#include <optional>
#include <d2d1_3.h>

namespace TitanEngine
{
    class Texture2D;

    // 유니티 Sprite에 대응하는 클래스.
    // Texture2D 레퍼런스 + 색조·명도·투명도 제어값을 담는다.
    class Sprite
    {
    public:
        std::shared_ptr<Texture2D> texture;

        // nullopt = 텍스처 전체. Animator가 프레임마다 설정한다.
        std::optional<D2D1_RECT_F> srcRect;

        // RGBA 각 채널 0~1. 흰색(1,1,1,1)이면 원본 그대로.
        D2D1_COLOR_F tint = { 1.f, 1.f, 1.f, 1.f };

        // 피벗 (0~1 정규화). {0.5, 0.5}가 중앙.
        D2D1_POINT_2F pivot = { 0.5f, 0.5f };

        ID2D1Bitmap1* GetBitmap() const;
        // srcRect가 설정된 경우 해당 영역의 크기를 반환
        UINT GetWidth()  const;
        UINT GetHeight() const;
    };
}
