#pragma once
#include "Animator.h"
#include "Component.h"
#include "InGameTimer.h"

using namespace TitanEngine;

class Player : public Component, public IUpdateable, public IRenderable
{
public:
	void OnAwake() override;
	void OnEnable() override;
	void OnStart() override;
	void Update(float deltaTime) override;
	void OnDisable() override;
	void OnDestory() override;

	// Component��(��) ���� ��ӵ�
	Object* Clone() override;

private:
	// IRenderable��(��) ���� ��ӵ�
	void Render(ID2D1DeviceContext7* ctx) override;

public:
	Animator* m_Animator;
	int animCnt = 0;

	InGameTimer* m_Timer;
	int m_AttackPower = 1;
};

