#pragma once
#include <unordered_set>
#include <string>
#include "SceneGraph.h"

namespace TitanEngine::SceneManagement
{
	class Scene abstract
	{
	public :
		Scene(std::string sceneName);
		virtual ~Scene();

		virtual void OnLoad() = 0;
		virtual void OnUnLoad() = 0;

		const std::string GetSceneName() { return m_sceneName; }
		SceneGraph* GetSceneGraph() { return m_sceneGraph; }

	protected :
		std::string m_sceneName;
		SceneGraph* m_sceneGraph = nullptr;
	};
}