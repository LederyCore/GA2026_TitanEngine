#include "pch.h"
#include "TitleScene.h"

#include <DebugConsole/DebugConsole.h>

#include "Button.h"
#include "SceneManager.h"

using namespace TitanEngine;

void TitleScene::OnLoad()
{

    auto* go = AddObject("Button");
    go->GetTransform()->SetLocalPosition(-100, -200);


    // 기본 버튼
    auto* go22 = AddObject("Button2");
    go22->GetTransform()->SetLocalPosition(0, 0);

    auto* btn2 = go22->AddComponent<Button>();
    btn2->width = 160.f;
    btn2->height = 40.f;
    btn2->text = L"게임 시작";
    btn2->onClick = []() {
        // 클릭 처리
        LOG_DEBUG("버튼 클릭");
        SceneManager::Instance().LoadScene("InGameScene");
        };
}

void TitleScene::OnUnLoad()
{
}
