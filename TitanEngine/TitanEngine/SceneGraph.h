#pragma once

// 씬 자체의 계층구조를 담당하는 클래스

namespace TitanEngine::SceneManagement
{
	class SceneGraph final
	{
	public :
		SceneGraph() = default;
		~SceneGraph() = default;

		void FixedUpdate(float fixedTime);
		void Update(float deltaTime);
		void LateUpdate(float deltaTime);

	private :

	};
}