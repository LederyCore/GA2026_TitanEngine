#include "pch.h"
#include "TestSceneLES.h"
#include "GameObject.h"      // cpp에서 포함
#include "DebugConsole/DebugConsole.h"

class PlayerController : public TitanEngine::ComponentBase<PlayerController>
{
public:
    void Awake() override
    {
        LOG_DEBUG("PlayerController Awake");
    }

    void Update(float dt) override
    {
        auto* tr = GetGameObject()->GetComponent<TitanEngine::Transform>();
        tr->localPosition.x += 100.f * dt;

        // string → const char* 변환
        std::string msg = "X : " + std::to_string(tr->localPosition.x);
        LOG_DEBUG(msg.c_str());
    }
};

void TestSceneLES::OnLoad()
{
    LOG_DEBUG("TestSceneLES OnLoad");
    m_player = CreateGameObject("Player");
    m_player->AddComponent<PlayerController>();
}

void TestSceneLES::OnUnLoad()
{
    LOG_DEBUG("TestSceneLES OnUnLoad");
}