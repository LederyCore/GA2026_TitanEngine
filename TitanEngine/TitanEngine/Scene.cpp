#include "pch.h"
#include "Scene.h"
#include "GameObject.h"

// 네임스페이스 안에서 TitanEngine 타입을 편하게 쓰기 위해
using TitanEngine::GameObject;
using TitanEngine::Transform;

namespace TitanEngine::SceneManagement
{
    Scene::Scene(const std::string& sceneName)
        : m_sceneName(sceneName)
    {
        m_sceneGraph = std::make_unique<SceneGraph>();
        m_updateSystem = std::make_unique<UpdateSystem>();
        m_renderSystem = std::make_unique<RenderSystem>();
    }

    // ── GameObject 생성 ───────────────────────────────────────
    GameObject* Scene::CreateGameObject(const std::string& name)
    {
        auto  go = std::make_unique<GameObject>(name);
        auto* ptr = go.get();
        m_gameObjects.push_back(std::move(go));
        m_sceneGraph->AddToRoot(ptr, this);
        return ptr;
    }

    GameObject* Scene::CreateGameObject(const std::string& name,
        Transform* parent)
    {
        auto  go = std::make_unique<GameObject>(name);
        auto* ptr = go.get();
        m_gameObjects.push_back(std::move(go));

        if (parent && parent->gameObject())
        {
            parent->AddChild(&ptr->transform);
            ptr->OnEnterScene(this);
        }
        else
        {
            m_sceneGraph->AddToRoot(ptr, this);
        }
        return ptr;
    }

    // ── GameObject 소멸 ───────────────────────────────────────
    void Scene::DestroyGameObject(GameObject* go)
    {
        if (!go) return;

        go->transform.DetachFromParent();
        go->OnExitScene();

        auto it = std::find_if(m_gameObjects.begin(), m_gameObjects.end(),
            [go](const std::unique_ptr<GameObject>& owned) {
                return owned.get() == go;
            });

        if (it != m_gameObjects.end())
            m_gameObjects.erase(it);
    }
}