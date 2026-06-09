#include "pch.h"
#include "GameObject.h"
#include "TestScene.h"
#include "TestComponent.h"
#include "FollowComponent.h"
#include "DebugConsole/DebugConsole.h"

using namespace TitanEngine;

void TestScene::OnLoad()
{
    LOG_DEBUG("TestScene Load");

    // 부모 오브젝트
    GameObject* parent = AddObject("Parent");
    auto* tc = parent->AddComponent<TestComponent>();
    parent->GetTransform()->SetLocalPosition(100, 200);

    // 자식 오브젝트
    GameObject* child = AddObject("Child");
    child->GetTransform()->SetParent(parent->GetTransform());
    child->GetTransform()->SetLocalPosition(-100, 0);

    // 자식에 FollowComponent 추가
    //auto* fc = child->AddComponent<FollowComponent>();
    //fc->SetTarget(parent->GetTransform());
    //fc->SetDelay(0.1f);

    // 자식에 렌더 컴포넌트 추가 (색만 다르게)
    auto* childRender = child->AddComponent<TestComponent>();
    childRender->m_color = D2D1::ColorF(D2D1::ColorF::Red);
    childRender->m_radius = 15.0f;
    childRender->m_speed = 0.f;
}

void TestScene::OnUnLoad()
{
    LOG_DEBUG("TestScene UnLoad");
}