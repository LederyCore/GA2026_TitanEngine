#include "pch.h"
#include "Object.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"

namespace TitanEngine
{
    Object::Object()
    {
        m_currentScene = SceneManagement::SceneManager::Instance().GetActiveScene();
    }

    void Object::Destroy(Object* obj)
    {
        if (obj == nullptr) return;
        if (obj->m_currentScene != nullptr)
            obj->m_currentScene->RemoveObject(obj);
    }

    void Object::Destroy(Object* obj, float delay)
    {
        if (obj == nullptr) return;
        obj->m_pendingDestroy = true;
        obj->m_destroyDelay = delay;
    }

    Object* Object::Instantiate(Object* original)
    {
        return Instantiate(original, nullptr, false);
    }

    Object* Object::Instantiate(Object* original, Transform* parent)
    {
        return Instantiate(original, parent, false);
    }

    Object* Object::Instantiate(Object* original, Transform* parent, bool instantiateInWorldSpace)
    {
        if (original == nullptr) return nullptr;

        // 1. 복제
        Object* clone = original->Clone();

        // 2. 씬 획득
        SceneManagement::Scene* activeScene =
            SceneManagement::SceneManager::Instance().GetActiveScene();
        if (!activeScene) return clone;

        clone->m_currentScene = activeScene;

        // 3. GameObject면 씬에 등록
        GameObject* go = dynamic_cast<GameObject*>(clone);
        if (go)
        {
            activeScene->RegisterObject(go);

            // 4. 부모 설정
            if (parent != nullptr)
            {
                if (instantiateInWorldSpace)
                {
                    // 월드 위치 유지 → 부모 설정 후 월드 위치 보정
                    D2D1_POINT_2F worldPos = go->GetTransform()->GetWorldPosition();
                    float worldRot = go->GetTransform()->GetWorldRotation();
                    D2D1_POINT_2F worldScl = go->GetTransform()->GetWorldScale();

                    go->GetTransform()->SetParent(parent);

                    D2D1_POINT_2F localPos = parent->GetInverseWorldMatrix().TransformPoint(worldPos);
                    go->GetTransform()->SetLocalPosition(localPos.x, localPos.y);
                    go->GetTransform()->SetLocalRotation(worldRot - parent->GetWorldRotation());
                    go->GetTransform()->SetLocalScale(
                        worldScl.x / parent->GetWorldScale().x,
                        worldScl.y / parent->GetWorldScale().y
                    );
                }
                else
                {
                    // 로컬 위치 유지 → 그냥 부모만 설정
                    go->GetTransform()->SetParent(parent);
                }
            }
        }

        return clone;
    }
}