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

	Object* Clone() override;

private:
	auto Render(ID2D1DeviceContext7* ctx) -> void override;

public:
	float m_currSize = 3.0f;
	bool IsSweetSpot() const { return m_currSize >= 0.2f && m_currSize <= 0.5f; }
};


