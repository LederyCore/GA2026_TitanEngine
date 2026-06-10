#pragma once
#include "Transform.h"
#include "GameObject.h"
#include <string>
#include <vector>

namespace TitanEngine::SceneManagement
{
    class Scene
    {
    public:
        static constexpr int MAX_GAMEOBJECTS = 1024;

        Scene(const std::string& name) : m_sceneName(name)
        {
            m_transforms.reserve(MAX_GAMEOBJECTS);
            m_allGameObjects.reserve(MAX_GAMEOBJECTS);
        }

        virtual ~Scene() = default;
        virtual void OnLoad() = 0;
        virtual void OnUnLoad() = 0;

        void Clear();
        void FixedUpdate(float fixedTime);
        void PropagateWorldMatrix();
        void Update(float deltaTime);
        void LateUpdate(float deltaTime);
        void Render(ID2D1DeviceContext7* ctx, float screenWidth, float screenHeight);

        GameObject* AddObject(const std::string& name);
        void        RegisterObject(GameObject* go);
        void        RemoveObject(GameObject* obj);

        // 인덱스 기반 Transform 접근
        Transform* GetTransform(int index) { return &m_transforms[index]; }
        void        SwapTransforms(int a, int b);

        void RegisterComponent(Component* comp);
        void UnRegisterComponent(Component* comp);
        void AddToPendingStartList(Component* comp);
        void AddToFixedUpdateList(IFixedUpdateable* c);
        void AddToUpdateList(IUpdateable* c);
        void AddToLateUpdateList(ILateUpdateable* c);
        void AddToRenderList(IRenderable* c);

        const std::string& GetSceneName() const { return m_sceneName; }

    private:
        void FlushDestroyQueue();

    private:
        std::string m_sceneName;

        std::vector<Transform>                   m_transforms;      // 값타입, 캐시 친화적
        std::vector<std::unique_ptr<GameObject>> m_allGameObjects;  // 모든 GameObject 소유

        std::vector<Component*>        m_pendingStartList;
        std::vector<IFixedUpdateable*> m_fixedUpdateList;
        std::vector<IUpdateable*>      m_updateableList;
        std::vector<ILateUpdateable*>  m_lateUpdateList;
        std::vector<IRenderable*>      m_renderList;
        std::vector<GameObject*>       m_pendingDestroyList;
    };
}