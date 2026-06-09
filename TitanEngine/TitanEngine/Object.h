#pragma once
namespace TitanEngine
{
    namespace SceneManagement { class Scene; }
    class Transform;
    class Object
    {
    public:
        Object();
        virtual ~Object() = default;
        virtual Object* Clone() = 0;

        // Destroy
        static void Destroy(Object* obj);
        static void Destroy(Object* obj, float delay);  // 지연 삭제

        template<typename T>
        static T* Instantiate(T* original)
        {
            return static_cast<T*>(Instantiate(static_cast<Object*>(original)));
        }
        template<typename T>
        static T* Instantiate(T* original, Transform* parent)
        {
            return static_cast<T*>(Instantiate(static_cast<Object*>(original), parent));
        }
        template<typename T>
        static T* Instantiate(T* original, Transform* parent, bool instantiateInWorldSpace)
        {
            return static_cast<T*>(Instantiate(static_cast<Object*>(original), parent, instantiateInWorldSpace));
        }

    private:
        static Object* Instantiate(Object* original);
        static Object* Instantiate(Object* original, Transform* parent);
        static Object* Instantiate(Object* original, Transform* parent, bool instantiateInWorldSpace);

    public :
        bool m_pendingDestroy = false;
        float m_destroyDelay = 0.0f;

    protected:
        SceneManagement::Scene* m_currentScene = nullptr;

    private :


    };
}