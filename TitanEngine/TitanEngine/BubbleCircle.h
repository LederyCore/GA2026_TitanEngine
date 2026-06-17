#pragma once
#include "Component.h"
#include "InGameTimer.h"

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

	// Rhythm-game timing: the ring shrinks from 3.0 down to 0. The smaller
	// the ring when clicked, the better the timing - until it passes the
	// bubble and becomes a miss again.
	enum class Timing { Miss, Good, Perfect };
	Timing GetTiming() const
	{
		if (m_currSize >= 0.30f && m_currSize <= 0.55f) return Timing::Perfect;
		if (m_currSize >= 0.12f && m_currSize <= 0.85f) return Timing::Good;
		return Timing::Miss;
	}
	bool IsSweetSpot() const { return GetTiming() != Timing::Miss; }
	bool IsPerfect()   const { return GetTiming() == Timing::Perfect; }

	InGameTimer* m_Timer;
};


