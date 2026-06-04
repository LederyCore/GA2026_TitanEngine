#include "pch.h"
#include "Scene.h"
#include "SceneGraph.h"
#include "DebugConsole/DebugConsole.h"


TitanEngine::SceneManagement::Scene::Scene(std::string sceneName)
{
	m_sceneName = sceneName;
	m_sceneGraph = new SceneGraph();
}

TitanEngine::SceneManagement::Scene::~Scene()
{
	delete m_sceneGraph;
	m_sceneGraph = nullptr;
}