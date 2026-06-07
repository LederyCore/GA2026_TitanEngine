#pragma once
#include "Scene.h"

namespace TitanEngine { class GameObject; }

class TestSceneLES : public TitanEngine::SceneManagement::Scene
{
public:
    TestSceneLES(const std::string& name = "TestSceneLES") : Scene(name) {}
    void OnLoad()   override;
    void OnUnLoad() override;

private:
    TitanEngine::GameObject* m_player = nullptr;
};