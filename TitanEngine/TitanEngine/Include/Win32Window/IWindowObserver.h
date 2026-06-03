#pragma once
#include <windows.h>
#include "IObserver.h"
namespace Platform
{
    class IWindowObserver : public IObserver
    {
    public:
        virtual ~IWindowObserver() = default;

        // 전체 구독자용 — 모든 메시지를 raw로 수신
        virtual void OnMessage(UINT msg, WPARAM wParam, LPARAM lParam) {}

        // ── 텍스트 입력 (IME 포함) ─────────────────────────────
        // WM_KEYDOWN/UP과 달리 문자 조합 결과이므로 이벤트가 적합
        virtual void OnChar(WPARAM ch) {}

        // ── 윈도우 상태 ────────────────────────────────────────
        virtual void OnResize(int width, int height) {}
        virtual void OnMove(int x, int y) {}
        virtual void OnActivate(bool active) {}
        virtual void OnFocusGained() {}
        virtual void OnFocusLost() {}
        virtual void OnEnterSizeMove() {}
        virtual void OnExitSizeMove() {}
        virtual void OnDisplayChange(int width, int height) {}

        // ── 렌더링 ─────────────────────────────────────────────
        virtual void OnPaint() {}
        virtual void OnEraseBackground() {}
    };
}