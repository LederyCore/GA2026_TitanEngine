#pragma once
#include <string>
#include <vector>
#include <memory>
#include "SceneGraph.h"

namespace TitanEngine { class GameObject; }

namespace TitanEngine::SceneManagement
{
    class Scene abstract
    {
    public:
        Scene(const std::string& sceneName);
        virtual ~Scene();

        virtual void OnLoad() = 0;
        virtual void OnUnLoad() = 0;

        const std::string& GetSceneName()  const { return m_sceneName; }
        SceneGraph* GetSceneGraph() { return m_sceneGraph; }

        TitanEngine::GameObject* CreateGameObject(const std::string& name);
        TitanEngine::GameObject* CreateGameObject(const std::string& name,
            TitanEngine::Transform* parent);
        void                     DestroyGameObject(TitanEngine::GameObject* go);


    protected:
        std::string   m_sceneName;
        SceneGraph* m_sceneGraph = nullptr;

        std::vector<std::unique_ptr<TitanEngine::GameObject>> m_gameObjects;
    };
}