#pragma once
#include <string>
#include <unordered_map>
#include <queue>
#include <memory>

namespace TitanEngine::SceneManagement
{
    class Scene;

    class SceneManager
    {
    public :
        static SceneManager& Instance()
        {
            static SceneManager instance;
            return instance;
        }

        SceneManager(const SceneManager&) = delete;
        SceneManager& operator=(const SceneManager&) = delete;

        bool         Initialize();
        void         Shutdown();

        void         LoadScene(const std::string& sceneName);
        void         UnLoadScene(const std::string& sceneName);
        void         FlushFrame();

        Scene*       GetActiveScene() const { return m_currentScene.get(); }
        bool         HasActiveScene() const { return m_currentScene != nullptr; }

        bool         RegisterScene(const std::string& sceneName, std::shared_ptr<Scene> scene);
        bool         UnRegisterScene(const std::string& sceneName);

    private :
        SceneManager() = default;
        ~SceneManager() = default;

    private :
        std::unordered_map<std::string, std::shared_ptr<Scene>> m_scenes;
        std::shared_ptr<Scene>  m_currentScene = nullptr;
        std::string             m_nextSceneName;
        std::queue<std::string> m_removeQueue;

    };
}