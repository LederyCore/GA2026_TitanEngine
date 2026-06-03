// Input.h
#pragma once
#include "InputSystem.h"

// Unity: Input.GetKeyDown(KeyCode.Space) 와 동일한 사용법
class Input
{
public:
    // ── 키보드 ──────────────────────────────────────────────
    static bool GetKeyDown(UINT vk) { return Get().GetKeyDown(vk); }
    static bool GetKeyPressed(UINT vk) { return Get().GetKeyPressed(vk); }
    static bool GetKeyReleased(UINT vk) { return Get().GetKeyReleased(vk); }

    // ── 마우스 ──────────────────────────────────────────────
    static bool  GetMouseButton(int btn) { return Get().GetMouseDown(btn); }
    static bool  GetMouseButtonDown(int btn) { return Get().GetMousePressed(btn); }
    static bool  GetMouseButtonUp(int btn) { return Get().GetMouseReleased(btn); }
    static POINT GetMousePosition() { return Get().GetMousePos(); }
    static int   GetMouseDeltaX() { return Get().GetMouseDeltaX(); }
    static int   GetMouseDeltaY() { return Get().GetMouseDeltaY(); }
    static SHORT GetMouseScrollDelta() { return Get().GetMouseWheel(); }

private:
    static Platform::InputSystem& Get() { return Platform::InputSystem::Instance(); }
};