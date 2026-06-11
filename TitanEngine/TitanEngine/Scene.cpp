#include "pch.h"
#include "Scene.h"
#include "DebugConsole/DebugConsole.h"
#include <algorithm>
#include <functional>

namespace TitanEngine::SceneManagement
{
    void Scene::Clear()
    {
        m_pendingStartList.clear();
        m_fixedUpdateList.clear();
        m_updateableList.clear();
        m_lateUpdateList.clear();
        m_renderList.clear();
        m_pendingDestroyList.clear();

        m_allGameObjects.clear();  // unique_ptr 소멸 → ~GameObject() → ~Component() 자동
        m_transforms.clear();
    }

    void Scene::FixedUpdate(float fixedTime)
    {
        for (auto* c : m_fixedUpdateList)
            c->FixedUpdate(fixedTime);
    }

    void Scene::Update(float deltaTime)
    {
        // 1. Start 처리
        for (Component* comp : m_pendingStartList)
        {
            if (comp->GetActive())
                comp->OnStart();
        }
        m_pendingStartList.clear();

        // 2. 일반 업데이트
        for (auto* c : m_updateableList)
            c->Update(deltaTime);

        // 3. 지연 삭제 타이머
        for (auto& go : m_allGameObjects)
        {
            if (go->m_pendingDestroy)
            {
                go->m_destroyDelay -= deltaTime;
                if (go->m_destroyDelay <= 0.0f)
                    RemoveObject(go.get());
            }
        }


        FlushDestroyQueue();
    }

    void Scene::LateUpdate(float deltaTime)
    {
        for (auto* c : m_lateUpdateList)
            c->LateUpdate(deltaTime);
    }

    void Scene::Render(ID2D1DeviceContext7* ctx, float screenWidth, float screenHeight)
    {
        D2D1::Matrix3x2F screenTransform =
            D2D1::Matrix3x2F::Translation(screenWidth * 0.5f, screenHeight * 0.5f);

        ctx->SetTransform(screenTransform);

        for (auto* c : m_renderList)
            c->Render(ctx);

        ctx->SetTransform(D2D1::Matrix3x2F::Identity());
    }

    void Scene::RegisterComponent(Component* comp)
    {
        if (comp == nullptr) return;

        if (auto* c = dynamic_cast<IFixedUpdateable*>(comp))
            if (std::find(m_fixedUpdateList.begin(), m_fixedUpdateList.end(), c) == m_fixedUpdateList.end())
                m_fixedUpdateList.push_back(c);

        if (auto* c = dynamic_cast<IUpdateable*>(comp))
            if (std::find(m_updateableList.begin(), m_updateableList.end(), c) == m_updateableList.end())
                m_updateableList.push_back(c);

        if (auto* c = dynamic_cast<ILateUpdateable*>(comp))
            if (std::find(m_lateUpdateList.begin(), m_lateUpdateList.end(), c) == m_lateUpdateList.end())
                m_lateUpdateList.push_back(c);

        if (auto* c = dynamic_cast<IRenderable*>(comp))
            if (std::find(m_renderList.begin(), m_renderList.end(), c) == m_renderList.end())
                m_renderList.push_back(c);
    }

    void Scene::UnRegisterComponent(Component* comp)
    {
        if (comp == nullptr) return;

        if (auto* c = dynamic_cast<IFixedUpdateable*>(comp))
            m_fixedUpdateList.erase(
                std::remove(m_fixedUpdateList.begin(), m_fixedUpdateList.end(), c),
                m_fixedUpdateList.end());

        if (auto* c = dynamic_cast<IUpdateable*>(comp))
            m_updateableList.erase(
                std::remove(m_updateableList.begin(), m_updateableList.end(), c),
                m_updateableList.end());

        if (auto* c = dynamic_cast<ILateUpdateable*>(comp))
            m_lateUpdateList.erase(
                std::remove(m_lateUpdateList.begin(), m_lateUpdateList.end(), c),
                m_lateUpdateList.end());

        if (auto* c = dynamic_cast<IRenderable*>(comp))
            m_renderList.erase(
                std::remove(m_renderList.begin(), m_renderList.end(), c),
                m_renderList.end());
    }

    GameObject* Scene::AddObject(const std::string& name)
    {
        if ((int)m_transforms.size() >= MAX_GAMEOBJECTS)
        {
            LOG_ERROR("MAX_GAMEOBJECTS(%d) 초과", MAX_GAMEOBJECTS);
            return nullptr;
        }

        int index = (int)m_transforms.size();

        // Transform 먼저 배열에 추가
        m_transforms.emplace_back();
        Transform* t = &m_transforms.back();
        t->m_selfIndex = index;

        // GameObject 생성
        std::unique_ptr<GameObject> go = std::make_unique<GameObject>(name);

        go->m_transform = t;
        t->m_owner = go.get();

        GameObject* goPtr = go.get();
        m_allGameObjects.push_back(std::move(go));

        return goPtr;
    }

    void Scene::SwapTransforms(int a, int b)
    {
        std::swap(m_transforms[a], m_transforms[b]);

        // selfIndex 갱신
        m_transforms[a].m_selfIndex = a;
        m_transforms[b].m_selfIndex = b;

        // m_owner->m_transform 포인터 갱신
        if (m_transforms[a].m_owner)
            m_transforms[a].m_owner->m_transform = &m_transforms[a];
        if (m_transforms[b].m_owner)
            m_transforms[b].m_owner->m_transform = &m_transforms[b];

        // 전체 배열의 parentIndex, childrenIndices 중 a↔b 교체
        for (Transform& t : m_transforms)
        {
            if (t.m_parentIndex == a)       t.m_parentIndex = b;
            else if (t.m_parentIndex == b)  t.m_parentIndex = a;

            for (int& ci : t.m_childrenIndices)
            {
                if (ci == a)      ci = b;
                else if (ci == b) ci = a;
            }
        }
    }

    void Scene::PropagateWorldMatrix()
    {
        for (int i = 0; i < (int)m_transforms.size(); i++)
        {
            Transform& t = m_transforms[i];

            Matrix local =
                Matrix::CreateScale(t.GetLocalScale().x, t.GetLocalScale().y, 1.0f)
                * Matrix::CreateRotationZ(DirectX::XMConvertToRadians(t.GetLocalRotation()))
                * Matrix::CreateTranslation(t.GetLocalPosition().x, t.GetLocalPosition().y, 0.0f);

            if (t.m_parentIndex == -1)
                t.SetWorldMatrix(local);  // 오프셋 없음
            else
                t.SetWorldMatrix(local * m_transforms[t.m_parentIndex].GetWorldMatrix());
        }
    }

    void Scene::RegisterObject(GameObject* go)
    {
        if (go == nullptr) return;
        if ((int)m_transforms.size() >= MAX_GAMEOBJECTS)
        {
            LOG_ERROR("MAX_GAMEOBJECTS(%d) 초과", MAX_GAMEOBJECTS);
            return;
        }

        int index = (int)m_transforms.size();

        m_transforms.emplace_back();
        Transform* t = &m_transforms.back();
        t->m_selfIndex = index;

        // 기존 Transform 값 복사
        *t = *go->m_transform;
        t->m_selfIndex = index;  // 복사 후 인덱스 재설정
        go->m_transform = t;
        t->m_owner = go;

        m_allGameObjects.push_back(std::unique_ptr<GameObject>(go));
    }

    void Scene::FlushDestroyQueue()
    {
        std::vector<int> toRemoveIndices;

        std::function<void(int)> collectWithChildren = [&](int idx)
            {
                if (std::find(toRemoveIndices.begin(), toRemoveIndices.end(), idx) != toRemoveIndices.end())
                    return;
                toRemoveIndices.push_back(idx);
                for (int childIdx : m_transforms[idx].m_childrenIndices)
                    collectWithChildren(childIdx);
            };

        for (GameObject* obj : m_pendingDestroyList)
        {
            if (obj && obj->m_transform)
                collectWithChildren(obj->m_transform->m_selfIndex);
        }

        std::sort(toRemoveIndices.rbegin(), toRemoveIndices.rend());

        for (int idx : toRemoveIndices)
        {
            GameObject* go = m_transforms[idx].m_owner;

            if (go)
            {
                // GO가 소유한 컴포넌트들을 각 리스트에서 제거
                for (auto& comp : go->GetComponents())
                {
                    Component* c = comp;

                    // PendingStart 리스트에서 제거
                    m_pendingStartList.erase(
                        std::remove(m_pendingStartList.begin(), m_pendingStartList.end(), c),
                        m_pendingStartList.end());

                    // 인터페이스별 리스트에서 제거
                    UnRegisterComponent(c);
                }
            }

            // Transform 제거
            m_transforms.erase(m_transforms.begin() + idx);

            // 인덱스 재정비
            for (int i = idx; i < (int)m_transforms.size(); i++)
            {
                m_transforms[i].m_selfIndex = i;
                if (m_transforms[i].m_owner)
                    m_transforms[i].m_owner->m_transform = &m_transforms[i];
            }
            for (Transform& t : m_transforms)
            {
                if (t.m_parentIndex > idx)  t.m_parentIndex--;
                else if (t.m_parentIndex == idx) t.m_parentIndex = -1;

                for (int& ci : t.m_childrenIndices)
                    if (ci > idx) ci--;

                t.m_childrenIndices.erase(
                    std::remove(t.m_childrenIndices.begin(), t.m_childrenIndices.end(), idx),
                    t.m_childrenIndices.end());
            }

            // GameObject 소유권 해제 (unique_ptr 소멸 → ~Component 자동 호출)
            m_allGameObjects.erase(
                std::remove_if(m_allGameObjects.begin(), m_allGameObjects.end(),
                    [go](const std::unique_ptr<GameObject>& g) { return g.get() == go; }),
                m_allGameObjects.end());
        }

        m_pendingDestroyList.clear();
    }

    // 나머지 동일
    void Scene::RemoveObject(GameObject* obj) { m_pendingDestroyList.push_back(obj); }
    void Scene::AddToPendingStartList(Component* c) { m_pendingStartList.push_back(c); }
    void Scene::AddToFixedUpdateList(IFixedUpdateable* c) { m_fixedUpdateList.push_back(c); }
    void Scene::AddToUpdateList(IUpdateable* c) { m_updateableList.push_back(c); }
    void Scene::AddToLateUpdateList(ILateUpdateable* c) { m_lateUpdateList.push_back(c); }
    void Scene::AddToRenderList(IRenderable* c) { m_renderList.push_back(c); }
}