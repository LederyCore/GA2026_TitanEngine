#pragma once
#include "Scene.h"

namespace TitanEngine { class GameObject; }

class TestSceneLES : public TitanEngine::SceneManagement::Scene
{
public:
    using Scene::Scene;
    void OnLoad()   override;
    void OnUnLoad() override;

private:
    TitanEngine::GameObject* m_player = nullptr;
    TitanEngine::GameObject* m_weapon = nullptr;
    TitanEngine::GameObject* m_camera = nullptr;
};