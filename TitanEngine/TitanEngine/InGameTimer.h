#pragma once
#include "Button.h"
#include "Component.h"
#include "Slider.h"

using namespace TitanEngine;


class InGameTimer : public Component, public IUpdateable, public IRenderable
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
	void Render(ID2D1DeviceContext7* ctx) override;

public:
	Slider* m_Slider;
	Button* m_RestartBtn;
	Button* m_MainBtn;
public:
	float m_MaxTime = 5.f;
	float m_CurTime = m_MaxTime;
};


