#include "pch.h"
#include "Object.h"
#include "SceneManager.h"
#include "Scene.h"

namespace TitanEngine
{
    Object::Object()
    {
        m_currentScene = SceneManagement::SceneManager::Instance().GetActiveScene();
    }

    void Object::Destroy(Object* obj)
    {
        if (obj == nullptr) return;

        // 씬에서 제거 요청
        if (obj->m_currentScene != nullptr)
            obj->m_currentScene->RemoveObject(obj);
    }

    void Object::Destroy(Object* obj, float delay)
    {
        if (obj == nullptr) return;

        // 지연 삭제 예약 (씬이 매 프레임 체크)
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

        // 1. 복제 (파생 클래스의 Clone 호출)
        Object* clone = original->Clone();

        // 2. 씬 등록
        SceneManagement::Scene* activeScene =
            SceneManagement::SceneManager::Instance().GetActiveScene();
        clone->m_currentScene = activeScene;
        activeScene->AddObject(clone);

        // 3. Transform 부모 설정
        // (Transform 연결은 GameObject 레벨 문제라 여기선 parent만 보관)
        // instantiateInWorldSpace는 향후 Transform::SetParent()에서 처리

        return clone;
    }
}