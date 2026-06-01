#pragma once
#include <functional>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <stdexcept>

// ============================================================
//  MulticastDelegate<Args...>
//
//  C#의 multicast delegate + event를 C++로 구현한 라이브러리.
//
//  사용 예:
//      MulticastDelegate<int, int> Resized;
//
//      // 구독 (토큰 없이)
//      Resized += [](int w, int h) { ... };
//
//      // 구독 (나중에 해제하려면 토큰 보관)
//      auto token = Resized.Subscribe([](int w, int h) { ... });
//      Resized.Unsubscribe(token);
//
//      // 호출
//      Resized.Invoke(1280, 720);
//      Resized(1280, 720);          // operator() 동일
//
// ============================================================

// 구독 해제에 사용하는 토큰
struct DelegateToken
{
    uint64_t id = 0;
    bool IsValid() const { return id != 0; }
};

inline bool operator==(const DelegateToken& a, const DelegateToken& b)
{
    return a.id == b.id;
}


template<typename... Args>
class MulticastDelegate
{
public:
    using Handler = std::function<void(Args...)>;

    MulticastDelegate() = default;
    ~MulticastDelegate() = default;

    // 복사 금지 (핸들러 목록 공유는 의도치 않은 동작을 유발)
    MulticastDelegate(const MulticastDelegate&) = delete;
    MulticastDelegate& operator=(const MulticastDelegate&) = delete;

    // 이동은 허용
    MulticastDelegate(MulticastDelegate&&) = default;
    MulticastDelegate& operator=(MulticastDelegate&&) = default;


    // ── 구독 ────────────────────────────────────────────────

    // 토큰 반환 — 나중에 Unsubscribe 가능
    DelegateToken Subscribe(Handler handler)
    {
        if (!handler)
            throw std::invalid_argument("MulticastDelegate: null handler");

        DelegateToken token{ ++m_nextId };
        m_entries.push_back({ token.id, std::move(handler) });
        return token;
    }

    // 토큰 없이 등록 (해제 불필요할 때)
    MulticastDelegate& operator+=(Handler handler)
    {
        Subscribe(std::move(handler));
        return *this;
    }


    // ── 구독 해제 ────────────────────────────────────────────

    // 토큰으로 해제
    bool Unsubscribe(DelegateToken token)
    {
        if (!token.IsValid()) return false;

        if (m_invoking)
        {
            // 호출 중이면 즉시 삭제하지 않고 다음 Invoke 후 정리
            for (auto& e : m_entries)
            {
                if (e.id == token.id)
                {
                    e.pendingRemove = true;
                    return true;
                }
            }
            return false;
        }

        return RemoveById(token.id);
    }

    // -= 연산자 (토큰 기반)
    MulticastDelegate& operator-=(DelegateToken token)
    {
        Unsubscribe(token);
        return *this;
    }


    // ── 호출 ─────────────────────────────────────────────────

    void Invoke(Args... args) const
    {
        m_invoking = true;

        for (const auto& e : m_entries)
        {
            if (!e.pendingRemove)
                e.handler(args...);
        }

        m_invoking = false;

        // 호출 중에 예약된 삭제 처리
        Cleanup();
    }

    // Invoke를 operator()로도 호출 가능
    void operator()(Args... args) const
    {
        Invoke(std::forward<Args>(args)...);
    }


    // ── 유틸 ─────────────────────────────────────────────────

    void Clear()
    {
        if (m_invoking)
        {
            for (auto& e : m_entries)
                e.pendingRemove = true;
        }
        else
        {
            m_entries.clear();
        }
    }

    bool IsEmpty()  const { return ActiveCount() == 0; }
    int  Count()    const { return static_cast<int>(ActiveCount()); }

    // 등록된 핸들러가 있으면 true (if(Resized) 패턴)
    explicit operator bool() const { return !IsEmpty(); }

private:
    struct Entry
    {
        uint64_t id = 0;
        Handler  handler;
        bool     pendingRemove = false;
    };

    std::vector<Entry>  m_entries;
    mutable bool        m_invoking = false;
    uint64_t            m_nextId = 0;

    bool RemoveById(uint64_t id)
    {
        auto it = std::find_if(m_entries.begin(), m_entries.end(),
            [id](const Entry& e) { return e.id == id; });

        if (it == m_entries.end()) return false;

        m_entries.erase(it);
        return true;
    }

    void Cleanup() const
    {
        m_entries.erase(
            std::remove_if(m_entries.begin(), m_entries.end(),
                [](const Entry& e) { return e.pendingRemove; }),
            m_entries.end());
    }

    size_t ActiveCount() const
    {
        return std::count_if(m_entries.begin(), m_entries.end(),
            [](const Entry& e) { return !e.pendingRemove; });
    }
};