#include "pch.h"
#include "Scene.h"
#include "GameObject.h"

namespace TitanEngine::SceneManagement
{
    void Scene::AddObject(Object* obj)
    {
        GameObject* go = dynamic_cast<GameObject*>(obj);
        if (go == nullptr) return;

        m_root.push_back(std::unique_ptr<GameObject>(go));
    }

    void Scene::RemoveObject(Object* obj)
    {
        // 대기열에 쌓고 Update 끝나고 일괄 삭제
        m_pendingDestroyList.push_back(obj);
    }

    void Scene::Update(float deltaTime)
    {
        // 1. 일반 업데이트
        for (auto& go : m_updateableList)
            go->Update(deltaTime);

        // 2. 지연 삭제 타이머 처리
        for (auto& go : m_root)
        {
            if (go->m_pendingDestroy)
            {
                go->m_destroyDelay -= deltaTime;
                if (go->m_destroyDelay <= 0.0f)
                    RemoveObject(go.get());
            }
        }

        // 3. 대기열 일괄 삭제
        for (Object* obj : m_pendingDestroyList)
        {
            // 컴포넌트이면 OnDestory 호출
            Component* comp = dynamic_cast<Component*>(obj);
            if (comp != nullptr)
            {
                comp->OnDestory();
            }

            // GameObject이면 모든 컴포넌트에 OnDestory 호출
            GameObject* go = dynamic_cast<GameObject*>(obj);
            if (go != nullptr)
            {
                for (Component* c : go->m_components)
                {
                    c->OnDestory();
                }
            }

            // 업데이트 리스트에서 제거
            m_fixedUpdateList.erase(
                std::remove_if(m_fixedUpdateList.begin(), m_fixedUpdateList.end(),
                    [obj](IFixedUpdateable* c) {
                        return c == dynamic_cast<IFixedUpdateable*>(obj);
                    }),
                m_fixedUpdateList.end()
            );
            m_updateableList.erase(
                std::remove_if(m_updateableList.begin(), m_updateableList.end(),
                    [obj](IUpdateable* c) {
                        return c == dynamic_cast<IUpdateable*>(obj);
                    }),
                m_updateableList.end()
            );
            m_lateUpdateList.erase(
                std::remove_if(m_lateUpdateList.begin(), m_lateUpdateList.end(),
                    [obj](ILateUpdateable* c) {
                        return c == dynamic_cast<ILateUpdateable*>(obj);
                    }),
                m_lateUpdateList.end()
            );
            m_renderList.erase(
                std::remove_if(m_renderList.begin(), m_renderList.end(),
                    [obj](IRenderable* c) {
                        return c == dynamic_cast<IRenderable*>(obj);
                    }),
                m_renderList.end()
            );

            // 루트에서 제거
            m_root.erase(
                std::remove_if(m_root.begin(), m_root.end(),
                    [obj](const std::unique_ptr<GameObject>& go) {
                        return go.get() == dynamic_cast<GameObject*>(obj);
                    }),
                m_root.end()
            );
        }
        m_pendingDestroyList.clear();
    }

    void Scene::FixedUpdate(float fixedTime)
    {
        for (auto& go : m_fixedUpdateList)
            go->FixedUpdate(fixedTime);
    }

    void Scene::PropagateWorldMatrix()
    {

    }

    void Scene::LateUpdate(float deltaTime)
    {
        for (auto& go : m_lateUpdateList)
            go->LateUpdate(deltaTime);
    }

    void Scene::Render(ID2D1DeviceContext7* ctx)
    {
        for (auto& go : m_renderList)
            go->Render(ctx);
    }

    void Scene::AddToFixedUpdateList(IFixedUpdateable* c) { m_fixedUpdateList.push_back(c); }
    void Scene::AddToUpdateList(IUpdateable* c)           { m_updateableList.push_back(c); }
    void Scene::AddToLateUpdateList(ILateUpdateable* c)   { m_lateUpdateList.push_back(c); }
    void Scene::AddToRenderList(IRenderable* c)           { m_renderList.push_back(c); }
}