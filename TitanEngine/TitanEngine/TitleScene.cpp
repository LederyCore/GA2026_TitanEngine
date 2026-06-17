#include "pch.h"
#include "TitleScene.h"

#include <DebugConsole/DebugConsole.h>

#include "Button.h"
#include "ResourceManager.h"
#include "SceneManager.h"
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

void TitleScene::OnLoad()
{
    GameObject* bg = AddObject("bg");
    bg->AddComponent<SpriteRenderer>();

    auto bgTex = ResourceManager::Load<Texture2D>(L"Resource/GameTitle.webp");

    bg->GetComponent<SpriteRenderer>()->sprite.texture = bgTex;


    auto* go = AddObject("Button");
    go->GetTransform()->SetLocalPosition(-100, -200);


    // 기본 버튼
    auto* go22 = AddObject("Button2");
    go22->GetTransform()->SetLocalPosition(0, 50);

    auto* btn2 = go22->AddComponent<Button>();
    auto startTex = ResourceManager::Load<Texture2D>(L"Resource/GameStart.webp");
    btn2->image.texture = startTex;
    btn2->width = 160.f;
    btn2->height = 40.f;
    //btn2->text = L"게임 시작";
    btn2->onClick = []() {
        // 클릭 처리
        LOG_DEBUG("버튼 클릭");
        SceneManager::Instance().LoadScene("InGameScene");
        };
}

void TitleScene::OnUnLoad()
{
}
