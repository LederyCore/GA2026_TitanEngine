#include "pch.h"
#include "TestScene.h"
#include "DebugConsole/DebugConsole.h"
#include "TestComponent.h"

void TestScene::OnLoad()
{
    using namespace TitanEngine;

    LOG_DEBUG("TestScene ·ÎµåµÊ");

    auto* go = AddObject("TestObject");
    go->AddComponent<TestComponent>();
}

void TestScene::OnUnLoad()
{
    LOG_DEBUG("TestScene ¾ð·ÎµåµÊ");
}


