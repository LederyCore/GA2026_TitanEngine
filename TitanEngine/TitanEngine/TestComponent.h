#pragma once
#include "Component.h"

namespace TitanEngine
{
	class TestComponent : public Component, public IUpdateable, public IRenderable
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

		float m_radius = 30.0f;
		float m_speed = 500.0f; // 픽셀/초
		D2D1::ColorF m_color = D2D1::ColorF(D2D1::ColorF::Orange);

	private:


		// IRenderable을(를) 통해 상속됨
		void Render(ID2D1DeviceContext7* ctx) override;
	};
}