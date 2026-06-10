#include "pch.h"
#include "GameObject.h"
#include "TestScene.h"
#include "TestComponent.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include "DebugConsole/DebugConsole.h"

using namespace TitanEngine;

void TestScene::OnLoad()
{
    LOG_DEBUG("TestScene Load");

    GameObject* catObj = AddObject("Cat");
    catObj->GetTransform()->SetLocalPosition(400, 300);
    catObj->GetTransform()->SetLocalScale(0.5f, 0.5f);
    auto* sr = catObj->AddComponent<SpriteRenderer>();
    auto tex = ResourceManager::Load<Texture2D>(L"Resource/cat.png");
    if (tex)
    {
        sr->sprite.texture = tex;
        sr->sprite.tint = { 1.f, 0.5f, 0.5f, 1.f }; // red tint example
        LOG_DEBUG("cat.png loaded OK (%u x %u)", tex->GetWidth(), tex->GetHeight());
    }
    catObj->AddComponent<TestComponent>();


    // parent object
    GameObject* parent = AddObject("Parent");
    auto* tc = parent->AddComponent<TestComponent>();
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