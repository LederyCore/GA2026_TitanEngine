#pragma once
#include "BubbleCircle.h"
#include "Component.h"
#include "InGameTimer.h"
#include "Player.h"

using namespace TitanEngine;

class Bubble : public Component, public IUpdateable, public IRenderable
{
public:
	void OnAwake() override;
	void OnEnable() override;
	void OnStart() override;
	void Update(float deltaTime) override;
	void OnDisable() override;
	void OnDestory() override;

	Object* Clone() override;

	static void SetScreenSize(float w, float h) { s_screenW = w; s_screenH = h; }

private:
	void Render(ID2D1DeviceContext7* ctx) override;

public:
	Player*       m_Player      = nullptr;
	InGameTimer*  m_Timer       = nullptr;
	BubbleCircle* m_BubbleCircle = nullptr;
	float         m_hitRadius   = 40.f;

private:
	static float s_screenW;
	static float s_screenH;
};



