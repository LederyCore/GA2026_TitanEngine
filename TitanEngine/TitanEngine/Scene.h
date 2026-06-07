#pragma once
#include <string>
#include <vector>
#include <memory>
#include "SceneGraph.h"
#include "UpdateSystem.h"
#include "RenderSystem.h"
#include "SystemLocator.h"

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
        UpdateSystem* GetUpdateSystem() { return m_updateSystem; }
        RenderSystem* GetRenderSystem() { return m_renderSystem; }

        TitanEngine::GameObject* CreateGameObject(const std::string& name);
        void                     DestroyGameObject(TitanEngine::GameObject* go);

    protected:
        std::string   m_sceneName;
        SceneGraph* m_sceneGraph = nullptr;
        UpdateSystem* m_updateSystem = nullptr;
        RenderSystem* m_renderSystem = nullptr;

        std::vector<std::unique_ptr<TitanEngine::GameObject>> m_gameObjects;
    };
}