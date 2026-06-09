#pragma once
#include "Component.h"

namespace TitanEngine
{
	class TestComponent : public Component, public IUpdateable
	{
	public :
		void OnAwake() override;
		void OnEnable() override;
		void OnStart() override;
		void Update(float deltaTime) override;
		void OnDisable() override;
		void OnDestory() override;

		// Component을(를) 통해 상속됨
		Object* Clone() override;
	};
}