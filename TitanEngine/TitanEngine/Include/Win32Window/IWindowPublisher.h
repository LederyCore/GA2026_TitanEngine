#pragma once
#include "IPublisher.h"
#include "IWindowObserver.h"

namespace Platform
{


    class IWindowPublisher : public IPublisher<IWindowObserver>
    {
        // 메시지별 구독 메서드 정의
        virtual void AddObserver(UINT msg, IWindowObserver* obs) = 0;
        virtual void RemoveObserver(UINT msg, IWindowObserver* obs) = 0;

        // 메시지 발생 시 호출할 통지 메서드
        virtual void Notify(UINT msg, WPARAM wParam, LPARAM lParam) = 0;
    };
}