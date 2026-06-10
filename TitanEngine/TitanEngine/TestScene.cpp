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

using namespace TitanEngine;

void TestScene::OnLoad()
{
    LOG_DEBUG("TestScene Load");

    // ---- cat ----
    GameObject* catObj = AddObject("Cat");
    catObj->GetTransform()->SetLocalPosition(400, 300);
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
}

void TestScene::OnUnLoad()
{
    LOG_DEBUG("TestScene UnLoad");
}
