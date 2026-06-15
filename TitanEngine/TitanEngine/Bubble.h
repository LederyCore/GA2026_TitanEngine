#pragma once
#include "Component.h"
#include "InGameTimer.h"

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

	// Component을(를) 통해 상속됨
	Object* Clone() override;

private:
	// IRenderable을(를) 통해 상속됨
	void Render(ID2D1DeviceContext7* ctx) override;


};



