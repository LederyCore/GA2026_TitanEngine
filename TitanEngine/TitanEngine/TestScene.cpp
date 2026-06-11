#include "pch.h"
#include "GameObject.h"
#include "TestScene.h"
#include "TestComponent.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "AnimationClip.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include "DebugConsole/DebugConsole.h"
#include "Button.h"
#include "Slider.h"

using namespace TitanEngine;

void TestScene::OnLoad()
{
    LOG_DEBUG("TestScene Load");







    // ---- cat ----
    GameObject* catObj = AddObject("Cat");
    catObj->GetTransform()->SetLocalPosition(0, 0);
    catObj->GetTransform()->SetLocalScale(0.5f, 0.5f);
    auto* sr = catObj->AddComponent<SpriteRenderer>();
    auto tex = ResourceManager::Load<Texture2D>(L"Resource/cat.png");
    if (tex)
    {
        sr->sprite.texture = tex;
        sr->sprite.tint = { 1.f, 0.5f, 0.5f, 1.f };
        LOG_DEBUG("cat.png loaded OK (%u x %u)", tex->GetWidth(), tex->GetHeight());
    }
    catObj->AddComponent<TestComponent>();

    // ---- panda animation ----
    // PandaSpriteSheet.png : 1472 x 64 pixels, 23 frames (each 64 x 64)
    auto pandaTex = ResourceManager::Load<Texture2D>(L"Resource/PandaSpriteSheet.png");
    if (pandaTex)
    {
        LOG_DEBUG("PandaSpriteSheet.png loaded OK (%u x %u)", pandaTex->GetWidth(), pandaTex->GetHeight());

        auto clip = std::make_shared<AnimationClip>();
        clip->name = "panda_run";
        clip->loop = true;
        clip->SetTexture(pandaTex);
        clip->AddFrames(64, 64, 23, 2.0f);  // 64x64 px, 23 frames, 2 seconds total

        GameObject* pandaObj = AddObject("Panda");
        pandaObj->GetTransform()->SetLocalPosition(0, 0);
        pandaObj->GetTransform()->SetLocalScale(1.f, 1.f);

        pandaObj->AddComponent<SpriteRenderer>();

        auto* anim = pandaObj->AddComponent<Animator>();
        anim->AddClip(clip);
        anim->Play("panda_run");



    }

    // ---- parent / child ----
    GameObject* parent = AddObject("Parent");
    parent->AddComponent<TestComponent>();
    parent->GetTransform()->SetLocalPosition(100, 200);

    GameObject* child = AddObject("Child");
    child->GetTransform()->SetParent(parent->GetTransform());
    child->GetTransform()->SetLocalPosition(-100, 0);

    auto* childRender = child->AddComponent<TestComponent>();
    childRender->m_color = D2D1::ColorF(D2D1::ColorF::Red);
    childRender->m_radius = 15.0f;
    childRender->m_speed = 0.f;

    auto* go = AddObject("Button");
    go->GetTransform()->SetLocalPosition(-100, -200);

    // 스프라이트 사이즈 사용하는 버튼
    auto* btn = go->AddComponent<Button>();
    btn->image.texture = tex;
    btn->width = 160.f;
    btn->height = 40.f;
    btn->text = L"Click Me";
    btn->onClick = []() {
        // 클릭 처리
        LOG_DEBUG("버튼 클릭");
        };


    // 기본 버튼
    auto* go22 = AddObject("Button2");
    go22->GetTransform()->SetLocalPosition(100, 000);

    auto* btn2 = go22->AddComponent<Button>();
    btn2->width = 160.f;
    btn2->height = 40.f;
    btn2->text = L"Click Me";
    btn2->onClick = []() {
        // 클릭 처리
        LOG_DEBUG("버튼 클릭");
        };



    // ---- player animation ----
    auto playerTex1 = ResourceManager::Load<Texture2D>(L"Resource/Player_Idle.png");
    if (playerTex1)
    {
        LOG_DEBUG("Player.png loaded OK (%u x %u)", playerTex1->GetWidth(), playerTex1->GetHeight());

        auto clip22 = std::make_shared<AnimationClip>();
        clip22->name = "player_idle";
        clip22->loop = true;
        clip22->SetTexture(playerTex1);
        clip22->AddFrames(112, 80, 12, 1.0f); 

        GameObject* playerObj1 = AddObject("player_idle");
        playerObj1->GetTransform()->SetLocalPosition(200, 0);
        playerObj1->GetTransform()->SetLocalScale(3.f, 3.f);

        playerObj1->AddComponent<SpriteRenderer>();

        auto* anim = playerObj1->AddComponent<Animator>();
        anim->AddClip(clip22);
        anim->Play("player_idle");
    }

    // ---- player animation ----
    auto playerTex2 = ResourceManager::Load<Texture2D>(L"Resource/Player_Attack.png");
    if (playerTex2)
    {
        LOG_DEBUG("Player.png loaded OK (%u x %u)", playerTex2->GetWidth(), playerTex2->GetHeight());

        auto clip22 = std::make_shared<AnimationClip>();
        clip22->name = "Player_Attack";
        clip22->loop = true;
        clip22->SetTexture(playerTex2);
        clip22->AddFrames(112, 80, 13, 1.0f);

        GameObject* playerObj2 = AddObject("Player_Attack");
        playerObj2->GetTransform()->SetLocalPosition(-200, 0);
        playerObj2->GetTransform()->SetLocalScale(3.f, 3.f);

        playerObj2->AddComponent<SpriteRenderer>();

        auto* anim = playerObj2->AddComponent<Animator>();
        anim->AddClip(clip22);
        anim->Play("Player_Attack");
    }


    // ---- player animation ----
    auto effect1 = ResourceManager::Load<Texture2D>(L"Resource/Effect_Hit.png");
    if (effect1)
    {
        LOG_DEBUG("Effect_Hit.png loaded OK (%u x %u)", effect1->GetWidth(), effect1->GetHeight());

        auto clip223 = std::make_shared<AnimationClip>();
        clip223->name = "Effect_Hit";
        clip223->loop = true;
        clip223->SetTexture(effect1);
        clip223->AddFrames(96, 96, 7, 0.4f);

        GameObject* effectObj1 = AddObject("Effect_Hit");
        effectObj1->GetTransform()->SetLocalPosition(-300, -100);
        effectObj1->GetTransform()->SetLocalScale(1.f, 1.f);

        effectObj1->AddComponent<SpriteRenderer>();

        auto* anim = effectObj1->AddComponent<Animator>();
        anim->AddClip(clip223);
        anim->Play("Effect_Hit");
    }

    // ---- player animation ----
    auto monster1Tex = ResourceManager::Load<Texture2D>(L"Resource/Monster_Idle.png");
    if (monster1Tex)
    {
        LOG_DEBUG("Monster_Idle.png loaded OK (%u x %u)", monster1Tex->GetWidth(), monster1Tex->GetHeight());

        auto clip223 = std::make_shared<AnimationClip>();
        clip223->name = "Monster_Idle";
        clip223->loop = true;
        clip223->SetTexture(monster1Tex);
        clip223->AddFrames(128, 64, 10, 1.f);

        GameObject* m1 = AddObject("Monster_Idle");
        m1->GetTransform()->SetLocalPosition(-300, -300);
        m1->GetTransform()->SetLocalScale(1.5f, 1.5f);

        m1->AddComponent<SpriteRenderer>();

        auto* anim = m1->AddComponent<Animator>();
        anim->AddClip(clip223);
        anim->Play("Monster_Idle");
    }

    // ---- player animation ----
    auto monster2Tex = ResourceManager::Load<Texture2D>(L"Resource/Monster_Attack.png");
    if (monster2Tex)
    {
        LOG_DEBUG("Monster_Attack.png loaded OK (%u x %u)", monster2Tex->GetWidth(), monster2Tex->GetHeight());

        auto clip223 = std::make_shared<AnimationClip>();
        clip223->name = "Monster_Attack";
        clip223->loop = true;
        clip223->SetTexture(monster2Tex);
        clip223->AddFrames(128, 64, 17, 1.f);

        GameObject* m2 = AddObject("Monster_Attack");
        m2->GetTransform()->SetLocalPosition(300, -300);
        m2->GetTransform()->SetLocalScale(1.5f, 1.5f);

        m2->AddComponent<SpriteRenderer>();

        auto* anim = m2->AddComponent<Animator>();
        anim->AddClip(clip223);
        anim->Play("Monster_Attack");
    }


    auto* hpgo = AddObject("HPBar");
    hpgo->GetTransform()->SetLocalPosition(0, -250);

    auto* hp = hpgo->AddComponent<Slider>();
    hp->minValue = 0.f;
    hp->maxValue = 100.f;
    hp->SetValue(75.f);               // HP 75/100
    hp->width = 300.f;
    hp->height = 24.f;
    hp->fillColor = D2D1::ColorF(0.2f, 0.85f, 0.2f, 1.f);  // 초록

    hp->onValueChanged = [](float v) {

        LOG_DEBUG("체력 변화");
        // HP 변경 감지
        };
}

void TestScene::OnUnLoad()
{
    LOG_DEBUG("TestScene UnLoad");
}
