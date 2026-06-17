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

	Object* Clone() override;

private:
	
	void Render(ID2D1DeviceContext7* ctx) override;

public:
	Slider* m_Slider;
	Button* m_RestartBtn;
	Button* m_MainBtn;
public:
	float m_MaxTime = 30.f;
	float m_CurTime = m_MaxTime;

	void GameClear();
	bool m_isGameClear;
	bool IsGameClear() const { return m_isGameClear; }

	std::shared_ptr<Texture2D> m_winTex;
	std::shared_ptr<Texture2D> m_loseTex;
	std::shared_ptr<Texture2D> m_TimerBarTex;

	GameObject* loseGo = nullptr;
};


