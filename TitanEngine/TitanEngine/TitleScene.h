#pragma once
#include "Scene.h"

using namespace TitanEngine::SceneManagement;

class TitleScene : public Scene
{
	using Scene::Scene;

	// Scene을(를) 통해 상속됨
	void OnLoad() override;
	void OnUnLoad() override;
};

