#pragma once
#include <memory>
#include <vector>
#include "AnimationClip.h"
#include "Component.h"
#include "InGameTimer.h"
#include "Player.h"
#include "Texture2D.h"

using namespace TitanEngine;

class BubbleSpawner : public Component, public IUpdateable
{
public:
	void OnAwake() override;
	void OnEnable() override;
	void OnStart() override;
	void Update(float deltaTime) override;
	void OnDisable() override;
	void OnDestory() override;

	Object* Clone() override;

public:
	Player*                     m_Player    = nullptr;
	InGameTimer*                m_Timer     = nullptr;
	std::shared_ptr<Texture2D>      m_BubbleTex;
	std::shared_ptr<Texture2D>      m_CircleTex;
	std::shared_ptr<AnimationClip>  m_BubbleHitClip;

	float m_MinInterval = 1.0f;
	float m_MaxInterval = 3.0f;

	// Register a no-spawn rectangle (world space). cx/cy = center, halfW/halfH =
	// half extents of the art, margin = extra padding (e.g. the bubble hit radius)
	// so a bubble's click area never overlaps the rect.
	void AddExclusionRect(float cx, float cy, float halfW, float halfH, float margin);

private:
	struct ExclusionZone { float minX, minY, maxX, maxY; };
	std::vector<ExclusionZone> m_Exclusions;

	float m_NextSpawnTimer = 0.0f;

	void  SpawnBubble();
	float RandomInterval();
	float GetProgress() const;          // 0 at match start -> 1 at the end
	bool  IsBlocked(float x, float y) const;
};
