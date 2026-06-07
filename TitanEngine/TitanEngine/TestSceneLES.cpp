#include "pch.h"
#include "TestSceneLES.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "DebugConsole/DebugConsole.h"
#include "InputSystem/InputSystem.h"

using namespace TitanEngine;
using namespace Platform;

class PlayerController : public ComponentBase<PlayerController>
{
public:
    float moveSpeed = 300.f;

    void Awake() override
    {
        LOG_DEBUG("[PlayerController] Awake");
    }

    void OnEnable() override
    {
        LOG_DEBUG("[PlayerController] OnEnable");
    }

    void OnDisable() override
    {
        LOG_DEBUG("[PlayerController] OnDisable");
    }

    void OnDestroy() override
    {
        LOG_DEBUG("[PlayerController] OnDestroy");
    }

    void Update(float dt) override
    {
        auto* tr = GetGameObject()->GetComponent<Transform>();

        if (InputSystem::Instance().GetKeyDown('A'))
            tr->localPosition.x -= moveSpeed * dt;
        if (InputSystem::Instance().GetKeyDown('D'))
            tr->localPosition.x += moveSpeed * dt;
        if (InputSystem::Instance().GetKeyDown('W'))
            tr->localPosition.y -= moveSpeed * dt;
        if (InputSystem::Instance().GetKeyDown('S'))
            tr->localPosition.y += moveSpeed * dt;

        std::string pos = "[Player] X: "
            + std::to_string(tr->localPosition.x)
            + " Y: "
            + std::to_string(tr->localPosition.y);
        LOG_DEBUG(pos.c_str());

        if (InputSystem::Instance().GetKeyPressed('E'))
        {
            SetEnabled(!IsEnabled());
            LOG_DEBUG(IsEnabled()
                ? "[PlayerController] Enabled"
                : "[PlayerController] Disabled");
        }

        if (InputSystem::Instance().GetKeyPressed('F'))
        {
            GetGameObject()->SetActive(!GetGameObject()->IsActive());
            LOG_DEBUG(GetGameObject()->IsActive()
                ? "[Player GO] Activated"
                : "[Player GO] Deactivated");
        }
    }
};

class CameraFollow : public ComponentBase<CameraFollow>
{
public:
    Transform* target = nullptr;
    float      smooth = 5.f;

    void Awake() override
    {
        LOG_DEBUG("[CameraFollow] Awake");
    }

    void LateUpdate(float dt) override
    {
        if (!target) return;

        auto* tr = GetGameObject()->GetComponent<Transform>();
        tr->localPosition.x +=
            (target->localPosition.x - tr->localPosition.x) * smooth * dt;
        tr->localPosition.y +=
            (target->localPosition.y - tr->localPosition.y) * smooth * dt;

        std::string msg = "[Camera] X: "
            + std::to_string(tr->localPosition.x)
            + " Y: "
            + std::to_string(tr->localPosition.y);
        LOG_DEBUG(msg.c_str());
    }
};

void TestSceneLES::OnLoad()
{
    LOG_DEBUG("── TestSceneLES OnLoad ──");

    m_player = CreateGameObject("Player");
    m_player->transform.localPosition = { 400.f, 300.f };
    m_player->AddComponent<PlayerController>();

    auto* ctrl = m_player->GetComponent<PlayerController>();
    LOG_DEBUG(ctrl
        ? "[GetComponent] PlayerController 조회 성공"
        : "[GetComponent] PlayerController 조회 실패");

    auto* tr = m_player->GetComponent<Transform>();
    LOG_DEBUG(tr
        ? "[GetComponent<Transform>] 특수화 조회 성공"
        : "[GetComponent<Transform>] 특수화 조회 실패");

    m_weapon = CreateGameObject("Weapon", &m_player->transform);
    m_weapon->transform.localPosition = { 50.f, 0.f };

    auto& sr = m_weapon->AddComponent<SpriteRenderer>();
    sr.size = { 64.f, 64.f };
    sr.sortingLayer = 0;
    sr.orderInLayer = 1;

    m_camera = CreateGameObject("Camera");
    auto& cam = m_camera->AddComponent<CameraFollow>();
    cam.target = &m_player->transform;

    auto controllers = m_player->GetComponents<PlayerController>();
    std::string msg = "[GetComponents] PlayerController count: "
        + std::to_string(controllers.size());
    LOG_DEBUG(msg.c_str());

    LOG_DEBUG("── TestSceneLES OnLoad 완료 ──");
}

void TestSceneLES::OnUnLoad()
{
    LOG_DEBUG("── TestSceneLES OnUnLoad ──");
}