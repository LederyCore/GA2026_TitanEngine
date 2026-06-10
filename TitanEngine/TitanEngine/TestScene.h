#pragma once
#include "Scene.h"

using namespace TitanEngine::SceneManagement;

class TestScene : public Scene
{
public :
	using Scene::Scene;

	// Scene을(를) 통해 상속됨
	void OnLoad() override;
	void OnUnLoad() override;

};