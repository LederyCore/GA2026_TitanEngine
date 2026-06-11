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
#include "Slider.h"
#include "SpriteRenderer.h"

using namespace TitanEngine;

void InGameScene::OnLoad()
{
    GameObject* enemy = AddObject("Enemy");
    enemy->AddComponent<Enemy>();
    enemy->AddComponent<SpriteRenderer>();
    auto* enemyAnim = enemy->AddComponent<Animator>();

    auto enemyIdleTex = ResourceManager::Load<Texture2D>(L"Resource/Monster_Idle.png");
    if (enemyIdleTex)
    {
        auto Idle = std::make_shared<AnimationClip>();
        Idle->name = "enemy_idle";
        Idle->loop = true;
        Idle->SetTexture(enemyIdleTex);
        Idle->AddFrames(128, 64, 10, 1.0f);  

        enemyAnim->AddClip(Idle);
        enemyAnim->Play("enemy_idle");
    }

    enemy->GetTransform()->SetLocalPosition(100, 0);
    enemy->GetTransform()->SetLocalScale(-3, 3);

    auto* healthSlider = AddObject("HealthSlider");
    healthSlider->AddComponent<Slider>();
    healthSlider->GetTransform()->SetLocalPosition(0, -200);
    
    enemy->GetComponent<Enemy>()->m_Slider = healthSlider->GetComponent<Slider>();

    // ========================================================================================================
    
	GameObject* player = AddObject("Player");
    player->AddComponent<Player>();
    player->AddComponent<SpriteRenderer>();
    auto* playerAnim = player->AddComponent<Animator>();

    auto playerIdleTex = ResourceManager::Load<Texture2D>(L"Resource/Player_Idle.png");
    if (playerIdleTex)
    {
        auto clip = std::make_shared<AnimationClip>();
        clip->name = "player_idle";
        clip->loop = true;
        clip->SetTexture(playerIdleTex);
        clip->AddFrames(112, 80, 12, 1.0f); 

        playerAnim->AddClip(clip);
        playerAnim->Play(clip->name);
    }

    auto playerAttack1Tex = ResourceManager::Load<Texture2D>(L"Resource/Player_Attack.png");
    if (playerAttack1Tex)
    {
        auto clip = std::make_shared<AnimationClip>();
        clip->name = "player_attack1";
        clip->loop = false;
        clip->SetTexture(playerAttack1Tex);
        clip->AddFrames(112, 80, 13, 0.2f);

        playerAnim->AddClip(clip);
    }

    auto playerAttack2Tex = ResourceManager::Load<Texture2D>(L"Resource/Player_Attack2.png");
    if (playerAttack2Tex)
    {
        auto clip = std::make_shared<AnimationClip>();
        clip->name = "player_attack2";
        clip->loop = false;
        clip->SetTexture(playerAttack2Tex);
        clip->AddFrames(112, 80, 12, 0.2f);

        playerAnim->AddClip(clip);
    }

    auto playerAttack3Tex = ResourceManager::Load<Texture2D>(L"Resource/Player_Attack3.png");
    if (playerAttack3Tex)
    {
        auto clip = std::make_shared<AnimationClip>();
        clip->name = "player_attack3";
        clip->loop = false;
        clip->SetTexture(playerAttack3Tex);
        clip->AddFrames(112, 80, 15, 0.2f);

        playerAnim->AddClip(clip);
    }

    player->GetTransform()->SetLocalPosition(-100, 0);
    player->GetTransform()->SetLocalScale(3, 3);

    
    // ========================================================================================================

    // 게임 승패 구분하기 위한 타이머
    GameObject* timer = AddObject("Timer");
    timer->AddComponent<InGameTimer>();
    
    auto* timerSlider = AddObject("TimerSlider");
    timerSlider->AddComponent<Slider>();
    timerSlider->GetTransform()->SetLocalPosition(0, -345);

    timer->GetComponent<InGameTimer>()->m_Slider = timerSlider->GetComponent<Slider>();
    timer->GetComponent<InGameTimer>()->m_Enemy = enemy->GetComponent<Enemy>();
}

void InGameScene::OnUnLoad()
{

}
