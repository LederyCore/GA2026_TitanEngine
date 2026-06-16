#include "pch.h"
#include "InGameTimer.h"

#include <DebugConsole/DebugConsole.h>

#include "Scene.h"

void InGameTimer::OnAwake()
{

}

void InGameTimer::OnEnable()
{

}

void InGameTimer::OnStart()
{
	m_Slider->minValue = 0.f;
	m_Slider->maxValue = m_MaxTime;
	m_Slider->SetValue(m_MaxTime);
	m_Slider->width = 1075.f;
	m_Slider->height = 24.f;
	m_Slider->fillColor = D2D1::ColorF(0.2f, 0.85f, 0.2f, 1.f);  // ÃÊ·Ï

	m_RestartBtn->SetActive(false);
	m_MainBtn->SetActive(false);
}

void InGameTimer::Update(float deltaTime)
{
	if (m_CurTime <= 0)
	{
		m_RestartBtn->SetActive(true);
		m_MainBtn->SetActive(true);
	}
	else
	{
		//LOG_DEBUG("%f", m_CurTime);
		if (m_isGameClear)
			return;

		m_CurTime -= deltaTime;
		m_Slider->SetValue(m_CurTime);
	}
}

void InGameTimer::OnDisable()
{

}

void InGameTimer::OnDestory()
{

}

Object* InGameTimer::Clone()
{
	return nullptr;
}

void InGameTimer::Render(ID2D1DeviceContext7* ctx)
{
}

void InGameTimer::GameClear()
{
	m_isGameClear = true;

	m_RestartBtn->SetActive(true);
	m_MainBtn->SetActive(true);
}
