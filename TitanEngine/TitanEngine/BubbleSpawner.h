#pragma once
#include <memory>
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
	std::shared_ptr<Texture2D>  m_BubbleTex;
	std::shared_ptr<Texture2D>  m_CircleTex;

	float m_MinInterval = 1.0f;
	float m_MaxInterval = 3.0f;

private:
	float m_NextSpawnTimer = 0.0f;

	void SpawnBubble();
	float RandomInterval();
};
