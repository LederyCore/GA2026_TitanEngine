#pragma once
#include "Transform.h"
#include <vector>

namespace TitanEngine::SceneManagement
{
	// 씬 자체의 계층구조를 담당하는 클래스
	class SceneGraph final
	{
	public :
		SceneGraph() = default;
		~SceneGraph() = default;

		void FixedUpdate(float fixedTime);
		void Update		(float deltaTime);
		void LateUpdate (float deltaTime);

	private :
		std::vector<Transform*> m_roots;
	};
}