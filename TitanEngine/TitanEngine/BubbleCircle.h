#pragma once
#include "Component.h"

using namespace TitanEngine;

class BubbleCircle : public Component, public IUpdateable, public IRenderable
{

public:
	void OnAwake() override;
	void OnEnable() override;
	void OnStart() override;
	void Update(float deltaTime) override;
	void OnDisable() override;
	void OnDestory() override;

	// Component을(를) 통해 상속됨
	Object* Clone() override;

private:
	// IRenderable을(를) 통해 상속됨
	auto Render(ID2D1DeviceContext7* ctx) -> void override;

	float m_currSize = 3;
};


