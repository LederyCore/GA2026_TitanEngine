#pragma once
#include "GameObject.h" 
#include <string>
#include <vector>
#include <memory>
namespace TitanEngine
{
    class IFixedUpdateable;
    class IUpdateable;
    class ILateUpdateable;
    class IRenderable;
    class Object;
}
namespace TitanEngine::SceneManagement
{
    class Scene
    {
    public:
        Scene(const std::string& name) : m_sceneName(name) {}
        virtual ~Scene() = default;
        virtual void OnLoad() = 0;
        virtual void OnUnLoad() = 0;
        void FixedUpdate(float fixedTime);
        void PropagateWorldMatrix();
        void Update(float deltaTime);
        void LateUpdate(float deltaTime);
        void Render(ID2D1DeviceContext7* ctx);
        void AddObject(Object* obj);
        void RemoveObject(Object* obj);
        void AddToFixedUpdateList(IFixedUpdateable* c);
        void AddToUpdateList(IUpdateable* c);
        void AddToLateUpdateList(ILateUpdateable* c);
        void AddToRenderList(IRenderable* c);
        const std::string GetSceneName() { return m_sceneName; }

    private:
        std::string m_sceneName;
        std::vector<std::unique_ptr<GameObject>> m_root;
        std::vector<IFixedUpdateable*> m_fixedUpdateList;
        std::vector<IUpdateable*>      m_updateableList;
        std::vector<ILateUpdateable*>  m_lateUpdateList;
        std::vector<IRenderable*>      m_renderList;
        std::vector<Object*>           m_pendingDestroyList;
    };
}