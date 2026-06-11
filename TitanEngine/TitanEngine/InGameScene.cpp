#include "pch.h"
#include "InGameScene.h"

#include <DebugConsole/DebugConsole.h>

#include "Animator.h"
#include "Enemy.h"
#include "Player.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include "AnimationClip.h"
#include "InGameTimer.h"
#include "SpriteRenderer.h"

using namespace TitanEngine;

void InGameScene::OnLoad()
{
    GameObject* enemy = AddObject("Enemy");
    enemy->AddComponent<Enemy>();

    // ---- panda animation ----
   // PandaSpriteSheet.png : 1472 x 64 pixels, 23 frames (each 64 x 64)
    auto pandaTex = ResourceManager::Load<Texture2D>(L"Resource/PandaSpriteSheet.png");
    if (pandaTex)
    {
        LOG_DEBUG("PandaSpriteSheet.png loaded OK (%u x %u)", pandaTex->GetWidth(), pandaTex->GetHeight());

        auto Idle = std::make_shared<AnimationClip>();
        Idle->name = "panda_run";
        Idle->loop = true;
        Idle->SetTexture(pandaTex);
        Idle->AddFrames(64, 64, 23, 2.0f);  // 64x64 px, 23 frames, 2 seconds total

        auto hit = std::make_shared<AnimationClip>();
        hit->name = "panda_hit";
        hit->loop = false;
        hit->SetTexture(pandaTex);
        hit->AddFrames(64, 64, 23, 0.5f);  // 64x64 px, 23 frames, 2 seconds total

        enemy->AddComponent<SpriteRenderer>();

        auto* anim = enemy->AddComponent<Animator>();
        anim->AddClip(Idle);
        anim->AddClip(hit);
        anim->Play("panda_run");

    }

    // 게임 승패 구분하기 위한 타이머
    GameObject* timer = AddObject("Timer");
    timer->AddComponent<InGameTimer>();


}

void InGameScene::OnUnLoad()
{

}
