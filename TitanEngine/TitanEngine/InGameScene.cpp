#include "pch.h"
#include "InGameScene.h"

#include <random>
#include <DebugConsole/DebugConsole.h>

#include "AnimationClip.h"
#include "Animator.h"
#include "Enemy.h"
#include "Player.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include "BubbleSpawner.h"
#include "Button.h"
#include "InGameTimer.h"
#include "SceneManager.h"
#include "Slider.h"
#include "SpriteRenderer.h"

using namespace TitanEngine;

void InGameScene::OnLoad()
{
    GameObject* enemy = AddObject("Enemy");
    enemy->AddComponent<Enemy>();
    enemy->AddComponent<SpriteRenderer>();
    auto* enemyAnim = enemy->AddComponent<Animator>();

    auto hitEffectTex = ResourceManager::Load<Texture2D>(L"Resource/Effect_Hit.png");
    if (hitEffectTex)
    {
        UINT frameW = hitEffectTex->GetWidth() / 7;
        UINT frameH = hitEffectTex->GetHeight();

        auto hitClip = std::make_shared<AnimationClip>();
        hitClip->name = "hit_effect";
        hitClip->loop = false;
        hitClip->SetTexture(hitEffectTex);
        hitClip->AddFrames(frameW, frameH, 7, 0.4f);

        enemy->GetComponent<Enemy>()->m_HitEffectClip = hitClip;
    }

    auto enemyIdleTex = ResourceManager::Load<Texture2D>(L"Resource/Monster_Idle.png");
    if (enemyIdleTex)
    {
        UINT frameW = enemyIdleTex->GetWidth() / 10;
        UINT frameH = enemyIdleTex->GetHeight();
        
        auto Idle = std::make_shared<AnimationClip>();
        Idle->name = "enemy_idle";
        Idle->loop = true;
        Idle->SetTexture(enemyIdleTex);
        Idle->AddFrames(frameW, frameH, 10, 1.0f);  

        enemyAnim->AddClip(Idle);
        enemyAnim->Play("enemy_idle");
    }

    enemy->GetTransform()->SetLocalPosition(100, 0);
    enemy->GetTransform()->SetLocalScale(-3, 3);

    auto* healthSlider = AddObject("HealthSlider");
    healthSlider->AddComponent<Slider>();
    healthSlider->GetTransform()->SetLocalPosition(0, -200);
    
    enemy->GetComponent<Enemy>()->m_Slider = healthSlider->GetComponent<Slider>();

    // 플레이어 ========================================================================================================
    
	GameObject* player = AddObject("Player");
    player->AddComponent<Player>();
    player->AddComponent<SpriteRenderer>();
    auto* playerAnim = player->AddComponent<Animator>();

    auto playerIdleTex = ResourceManager::Load<Texture2D>(L"Resource/Player_Idle.png");
    if (playerIdleTex)
    {
        UINT frameW = playerIdleTex->GetWidth() / 12;
        UINT frameH = playerIdleTex->GetHeight();
        
        auto clip = std::make_shared<AnimationClip>();
        clip->name = "player_idle";
        clip->loop = true;
        clip->SetTexture(playerIdleTex);
        clip->AddFrames(frameW, frameH, 12, 1.0f); 

        playerAnim->AddClip(clip);
        playerAnim->Play(clip->name);
    }

    auto playerAttack1Tex = ResourceManager::Load<Texture2D>(L"Resource/Player_Attack.png");
    if (playerAttack1Tex)
    {
        UINT frameW = playerAttack1Tex->GetWidth() / 13;
        UINT frameH = playerAttack1Tex->GetHeight();
        
        auto clip = std::make_shared<AnimationClip>();
        clip->name = "player_attack1";
        clip->loop = false;
        clip->SetTexture(playerAttack1Tex);
        clip->AddFrames(frameW, frameH, 13, 0.2f);

        playerAnim->AddClip(clip);
    }

    auto playerAttack2Tex = ResourceManager::Load<Texture2D>(L"Resource/Player_Attack2.png");
    if (playerAttack2Tex)
    {
        UINT frameW = playerAttack2Tex->GetWidth() / 12;
        UINT frameH = playerAttack2Tex->GetHeight();
        
        auto clip = std::make_shared<AnimationClip>();
        clip->name = "player_attack2";
        clip->loop = false;
        clip->SetTexture(playerAttack2Tex);
        clip->AddFrames(frameW, frameH, 12, 0.2f);

        playerAnim->AddClip(clip);
    }

    auto playerAttack3Tex = ResourceManager::Load<Texture2D>(L"Resource/Player_Attack3.png");
    if (playerAttack3Tex)
    {
        
        UINT frameW = playerAttack3Tex->GetWidth() / 15;
        UINT frameH = playerAttack3Tex->GetHeight();
        
        auto clip = std::make_shared<AnimationClip>();
        clip->name = "player_attack3";
        clip->loop = false;
        clip->SetTexture(playerAttack3Tex);
        clip->AddFrames(frameW, frameH, 15, 0.2f);

        playerAnim->AddClip(clip);
    }

    player->GetTransform()->SetLocalPosition(-100, 0);
    player->GetTransform()->SetLocalScale(3, 3);

    
    // 타이머========================================================================================================

   
    GameObject* timer = AddObject("Timer");
    timer->AddComponent<InGameTimer>();
    
    auto* timerSlider = AddObject("TimerSlider");
    timerSlider->AddComponent<Slider>();
    timerSlider->GetTransform()->SetLocalPosition(0, -345);

    timer->GetComponent<InGameTimer>()->m_Slider = timerSlider->GetComponent<Slider>();


    player->GetComponent<Player>()->m_Timer = timer->GetComponent<InGameTimer>();
    enemy->GetComponent<Enemy>()->m_Timer  = timer->GetComponent<InGameTimer>();
    enemy->GetComponent<Enemy>()->m_Player = player->GetComponent<Player>();


    auto* go = AddObject("Button");
    go->GetTransform()->SetLocalPosition(-100, -200);


    // 재시작 버튼
    auto* btnGo = AddObject("RestartButton");
    btnGo->GetTransform()->SetLocalPosition(0, 0);

    auto* btn_restart = btnGo->AddComponent<Button>();
    btn_restart->width = 160.f;
    btn_restart->height = 40.f;
    btn_restart->text = L"restart";
    btn_restart->onClick = []() {
        //LOG_DEBUG("재시작");
        SceneManager::Instance().LoadScene("InGameScene");
        };

    timer->GetComponent<InGameTimer>()->m_RestartBtn = btnGo->GetComponent<Button>();

    // 메인화면 버튼
    auto* btnGo2 = AddObject("RestartButton");
    btnGo2->GetTransform()->SetLocalPosition(0, 200);

    auto* btn_Main = btnGo2->AddComponent<Button>();
    btn_Main->width = 160.f;
    btn_Main->height = 40.f;
    btn_Main->text = L"Main";
    btn_Main->onClick = []() {
        LOG_DEBUG("btn_Main");
        SceneManager::Instance().LoadScene("TitleScene");
        };

    timer->GetComponent<InGameTimer>()->m_MainBtn = btnGo2->GetComponent<Button>();


    // 버블 스포너 ===============================================================================================
    auto bubbleTex    = ResourceManager::Load<Texture2D>(L"Resource/Bubble_Icon.png");
    auto circleTex    = ResourceManager::Load<Texture2D>(L"Resource/Bubble_ScaleCircle.png");
    auto bubbleHitTex = ResourceManager::Load<Texture2D>(L"Resource/Bubble_Hit.png");

    std::shared_ptr<AnimationClip> bubbleHitClip;
    if (bubbleHitTex)
    {
        UINT frameW = bubbleHitTex->GetWidth() / 7;
        UINT frameH = bubbleHitTex->GetHeight();

        bubbleHitClip = std::make_shared<AnimationClip>();
        bubbleHitClip->name = "bubble_hit";
        bubbleHitClip->loop = false;
        bubbleHitClip->SetTexture(bubbleHitTex);
        bubbleHitClip->AddFrames(frameW, frameH, 7, 0.4f);
    }

    GameObject* spawnerGO = AddObject("BubbleSpawner");
    auto* spawner = spawnerGO->AddComponent<BubbleSpawner>();
    spawner->m_Player        = player->GetComponent<Player>();
    spawner->m_Timer         = timer->GetComponent<InGameTimer>();
    spawner->m_BubbleTex     = bubbleTex;
    spawner->m_CircleTex     = circleTex;
    spawner->m_BubbleHitClip = bubbleHitClip;
    spawner->m_MinInterval   = 1.0f;
    spawner->m_MaxInterval   = 3.0f;
}

void InGameScene::OnUnLoad()
{

}
