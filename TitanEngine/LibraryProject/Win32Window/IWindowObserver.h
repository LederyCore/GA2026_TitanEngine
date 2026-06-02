#pragma once
#include <windows.h>
#include "IObserver.h"
namespace Platform
{
    class IWindowObserver : public IObserver
    {
    public:
        virtual ~IWindowObserver() = default;

        // ── 키보드 ──────────────────────────────────────────────────────────────
        // AddObserver(WM_KEYDOWN, this)
        virtual void OnKeyDown(WPARAM key) {}
        // AddObserver(WM_KEYUP, this)
        virtual void OnKeyUp(WPARAM key) {}
        // AddObserver(WM_SYSKEYDOWN, this)
        virtual void OnSysKeyDown(WPARAM key) {}
        // AddObserver(WM_SYSKEYUP, this)
        virtual void OnSysKeyUp(WPARAM key) {}
        // AddObserver(WM_CHAR, this)
        virtual void OnChar(WPARAM key) {}

        // ── 마우스 ──────────────────────────────────────────────────────────────
        // AddObserver(WM_MOUSEMOVE, this)
        virtual void OnMouseMove(int x, int y) {}
        // AddObserver(WM_LBUTTONDOWN, this)
        virtual void OnMouseLButtonDown(int x, int y) {}
        // AddObserver(WM_LBUTTONUP, this)
        virtual void OnMouseLButtonUp(int x, int y) {}
        // AddObserver(WM_RBUTTONDOWN, this)
        virtual void OnMouseRButtonDown(int x, int y) {}
        // AddObserver(WM_RBUTTONUP, this)
        virtual void OnMouseRButtonUp(int x, int y) {}
        // AddObserver(WM_MBUTTONDOWN, this)
        virtual void OnMouseMButtonDown(int x, int y) {}
        // AddObserver(WM_MBUTTONUP, this)
        virtual void OnMouseMButtonUp(int x, int y) {}
        // AddObserver(WM_MOUSEWHEEL, this)
        virtual void OnMouseWheel(int delta) {}
        // AddObserver(WM_XBUTTONDOWN, this)
        virtual void OnMouseXButtonDown(int x, int y, WORD button) {}
        // AddObserver(WM_XBUTTONUP, this)
        virtual void OnMouseXButtonUp(int x, int y, WORD button) {}
        // AddObserver(WM_LBUTTONDBLCLK, this)
        virtual void OnMouseLButtonDblClk(int x, int y) {}

        // ── 윈도우 상태 ─────────────────────────────────────────────────────────
        // AddObserver(WM_SIZE, this)
        virtual void OnResize(int width, int height) {}
        // AddObserver(WM_MOVE, this)
        virtual void OnMove(int x, int y) {}
        // AddObserver(WM_ACTIVATE, this)
        virtual void OnActivate(bool active) {}
        // AddObserver(WM_SETFOCUS, this)
        virtual void OnFocusGained() {}
        // AddObserver(WM_KILLFOCUS, this)
        virtual void OnFocusLost() {}
        // AddObserver(WM_ENTERSIZEMOVE, this)
        virtual void OnEnterSizeMove() {}
        // AddObserver(WM_EXITSIZEMOVE, this)
        virtual void OnExitSizeMove() {}
        // AddObserver(WM_DISPLAYCHANGE, this)
        virtual void OnDisplayChange(int width, int height) {}
        // AddObserver(WM_CLOSE, this)
        //virtual void OnClose() {}
        // AddObserver(WM_DESTROY, this)
        //virtual void OnDestroy() {}

        // ── 렌더링 ──────────────────────────────────────────────────────────────
        // AddObserver(WM_PAINT, this)
        virtual void OnPaint() {}
        // AddObserver(WM_ERASEBKGND, this)
        virtual void OnEraseBackground() {}
    };
}