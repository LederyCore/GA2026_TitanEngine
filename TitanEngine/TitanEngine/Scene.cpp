#include "pch.h"
#include "Scene.h"
#include "SceneGraph.h"
#include "UpdateSystem.h"
#include "RenderSystem.h"
#include "SystemLocator.h"
#include "GameObject.h"
#include <algorithm>

TitanEngine::SceneManagement::Scene::Scene(const std::string& sceneName)
{
    m_sceneName = sceneName;
    m_sceneGraph = new SceneGraph();
    m_updateSystem = new UpdateSystem();
    m_renderSystem = new RenderSystem();
    m_physicsSystem = new PhysicsSystem();

    SystemLocator::Set(m_updateSystem, m_renderSystem, m_physicsSystem);
}

TitanEngine::SceneManagement::Scene::~Scene()
{
    SystemLocator::Clear();

    delete m_sceneGraph;
    delete m_updateSystem;
    delete m_renderSystem;
    delete m_physicsSystem;
    m_sceneGraph = nullptr;
    m_updateSystem = nullptr;
    m_renderSystem = nullptr;
    m_physicsSystem = nullptr;
}

TitanEngine::GameObject* TitanEngine::SceneManagement::Scene::CreateGameObject(const std::string& name)
{
    auto  go = std::make_unique<TitanEngine::GameObject>(name);
    auto* ptr = go.get();

    m_sceneGraph->AddRoot(&ptr->transform);
    m_gameObjects.push_back(std::move(go));

    return ptr;
}

TitanEngine::GameObject* TitanEngine::SceneManagement::Scene::CreateGameObject(const std::string& name, TitanEngine::Transform* parent)
{
    auto  go = std::make_unique<TitanEngine::GameObject>(name);
    auto* ptr = go.get();

    if (parent)
        ptr->transform.SetParent(parent);
    else
        m_sceneGraph->AddRoot(&ptr->transform);

    m_gameObjects.push_back(std::move(go));
    return ptr;
}

void TitanEngine::SceneManagement::Scene::DestroyGameObject(TitanEngine::GameObject* go)
{
    m_sceneGraph->RemoveRoot(&go->transform);

    m_gameObjects.erase(
        std::remove_if(m_gameObjects.begin(), m_gameObjects.end(),
            [go](const std::unique_ptr<TitanEngine::GameObject>& p)
            {
                return p.get() == go;
            }),
        m_gameObjects.end());
}