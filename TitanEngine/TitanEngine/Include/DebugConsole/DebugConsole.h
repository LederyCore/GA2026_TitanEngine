#pragma once
#include <string>
#include <mutex>
#include <handleapi.h>

enum class LogType : uint8_t
{
    INFO = 0,
    WARNING = 1,
    ERR = 2,
    DEBUG = 3,
    NETWORK = 4,
    GAME = 5,
};

// =============================================
// Debug 빌드에서만 실제 구현 컴파일
// =============================================
#ifdef _DEBUG

class DebugConsole
{
public:
    static DebugConsole& GetInstance();

    void Info(const char* fmt, ...);
    void Warning(const char* fmt, ...);
    void Error(const char* fmt, ...);
    void Debug(const char* fmt, ...);
    void Network(const char* fmt, ...);
    void Game(const char* fmt, ...);
    void Log(LogType type, const char* fmt, ...);

    void SetEnabled(bool enabled);
    void SetFilter(LogType type, bool show);

private:
    DebugConsole();
    ~DebugConsole() = default;
    DebugConsole(const DebugConsole&) = delete;
    DebugConsole& operator=(const DebugConsole&) = delete;

    void        LogInternal(LogType type, const char* message);
    std::string GetTimeString() const;
    const char* GetTypeLabel(LogType type) const;
    void        SetColor(LogType type) const;
    void        ResetColor() const;

private:
    bool       m_enabled = true;
    bool       m_filter[6] = { true, true, true, true, true, true };
    HANDLE     m_hConsole = INVALID_HANDLE_VALUE;
    std::mutex m_mutex;
};

// Debug 빌드 - 실제 동작
#define LOG_INFO(...)    DebugConsole::GetInstance().Info   (__VA_ARGS__)
#define LOG_WARN(...)    DebugConsole::GetInstance().Warning(__VA_ARGS__)
#define LOG_ERROR(...)   DebugConsole::GetInstance().Error  (__VA_ARGS__)
#define LOG_DEBUG(...)   DebugConsole::GetInstance().Debug  (__VA_ARGS__)
#define LOG_NET(...)     DebugConsole::GetInstance().Network(__VA_ARGS__)
#define LOG_GAME(...)    DebugConsole::GetInstance().Game   (__VA_ARGS__)

#else

// =============================================
// Release 빌드 - 매크로가 아무것도 안 함
// 컴파일러가 완전히 제거 (zero cost)
// =============================================
#define LOG_INFO(...)    ((void)0)
#define LOG_WARN(...)    ((void)0)
#define LOG_ERROR(...)   ((void)0)
#define LOG_DEBUG(...)   ((void)0)
#define LOG_NET(...)     ((void)0)
#define LOG_GAME(...)    ((void)0)

#endif // _DEBUG