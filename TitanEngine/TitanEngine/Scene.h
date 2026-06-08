// Scene.h
#pragma once
#include <string>
#include <vector>
#include <memory>
#include "SceneGraph.h"
#include "UpdateSystem.h"
#include "RenderSystem.h"
#include "GameObject.h"   // ← 전방선언 대신 완전 포함으로 변경

namespace TitanEngine::SceneManagement
{
    class Scene abstract
    {
    public:
        explicit Scene(const std::string& sceneName);
        virtual ~Scene() = default;              // ← 선언만, cpp에서 정의
        virtual void OnLoad() = 0;
        virtual void OnUnLoad() = 0;

        const std::string& GetSceneName() const { return m_sceneName; }

        SceneGraph* GetSceneGraph() { return m_sceneGraph.get(); }
        UpdateSystem* GetUpdateSystem() { return m_updateSystem.get(); }
        RenderSystem* GetRenderSystem() { return m_renderSystem.get(); }

        ::TitanEngine::GameObject* CreateGameObject(const std::string& name);
        ::TitanEngine::GameObject* CreateGameObject(const std::string& name,
            ::TitanEngine::Transform* parent);
        void DestroyGameObject(::TitanEngine::GameObject* go);

    protected:
        std::string m_sceneName;

        std::unique_ptr<SceneGraph>   m_sceneGraph;
        std::unique_ptr<UpdateSystem> m_updateSystem;
        std::unique_ptr<RenderSystem> m_renderSystem;

        std::vector<std::unique_ptr<::TitanEngine::GameObject>> m_gameObjects;
    };
}