#pragma once
#include "Component.h"

namespace TitanEngine
{
	class TestComponent : Component
	{
	public :
		void OnStart() override;


		// Component을(를) 통해 상속됨
		Object* Clone() override;

	};
}