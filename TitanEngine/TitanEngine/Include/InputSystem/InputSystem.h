// InputSystem.h
#pragma once

inline int GetXFromLParam(LPARAM lp) { return (int)(short)(LOWORD(lp)); }
inline int GetYFromLParam(LPARAM lp) { return (int)(short)(HIWORD(lp)); }

struct MouseEdge
{
    bool pressed = false;
    bool released = false;
};

struct MouseState
{
    POINT pos{ 0, 0 };
    int   deltaX = 0;       // Raw Input 프레임 이동량
    int   deltaY = 0;
    SHORT wheelDelta = 0;       // 프레임 휠 누적량 (+120/-120 단위)
    bool  leftPressed = false;
    bool  rightPressed = false;
    bool  middlePressed = false;
};

struct KeyEdge
{
    bool pressed = false;
    bool released = false;
};

namespace Platform
{
    class InputSystem
    {
    public:
        static InputSystem& Instance()
        {
            static InputSystem instance;
            return instance;
        }

        bool Initialize(HWND hWnd);
        bool OnHandleMessage(const MSG& msg);
        void SetDebuging(bool value) { m_debugToggle = value; }

        // 매 프레임 Update() 직전에 호출
        void FlushFrame()
        {
            for (auto& e : m_keyEdge) { e.pressed = false; e.released = false; }
            for (auto& e : m_mouseEdge) { e.pressed = false; e.released = false; }
            m_curMouse.deltaX = 0;
            m_curMouse.deltaY = 0;
            m_curMouse.wheelDelta = 0;
        }

        // ── 키보드 ──────────────────────────────────────────
        bool GetKeyDown(UINT vk) const { return m_keyDown[vk]; }
        bool GetKeyPressed(UINT vk) const { return m_keyEdge[vk].pressed; }
        bool GetKeyReleased(UINT vk) const { return m_keyEdge[vk].released; }

        // ── 마우스 ──────────────────────────────────────────
        // btn: 0=Left, 1=Right, 2=Middle
        bool GetMouseDown(int btn) const { return btn == 0 ? m_curMouse.leftPressed : btn == 1 ? m_curMouse.rightPressed : m_curMouse.middlePressed; }
        bool GetMousePressed(int btn) const { return m_mouseEdge[btn].pressed; }
        bool GetMouseReleased(int btn) const { return m_mouseEdge[btn].released; }
        POINT GetMousePos()         const { return m_curMouse.pos; }
        int   GetMouseDeltaX()         const { return m_curMouse.deltaX; }
        int   GetMouseDeltaY()         const { return m_curMouse.deltaY; }
        SHORT GetMouseWheel()         const { return m_curMouse.wheelDelta; }
        MouseState GetMouseState()     const { return m_curMouse; }

    private:
        InputSystem() = default;
        ~InputSystem() = default;
        InputSystem(const InputSystem&) = delete;
        InputSystem& operator=(const InputSystem&) = delete;

        void HandleMsgKeyDown(WPARAM wParam, LPARAM lParam);
        void HandleMsgKeyUp(WPARAM wParam, LPARAM lParam);
        void HandleMsgMouse(const MSG& msg);
        void HandleRawInput(LPARAM lParam);
        void HandleKeyboardInput(RAWINPUT& raw);
        void HandleMouseInput(RAWINPUT& raw);

        MouseState               m_curMouse;
        bool                     m_debugToggle = true;
        std::array<MouseEdge, 3> m_mouseEdge = {};
        std::array<bool, 256>    m_keyDown = {};
        std::array<KeyEdge, 256> m_keyEdge = {};
    };
}