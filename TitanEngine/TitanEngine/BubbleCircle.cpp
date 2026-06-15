#include "pch.h"
#include "BubbleCircle.h"

#include <DebugConsole/DebugConsole.h>
#include <InputSystem/InputSystem.h>

#include "GameObject.h"


void BubbleCircle::OnAwake()
{
}

void BubbleCircle::OnEnable()
{
}

void BubbleCircle::OnStart()
{
	LOG_DEBUG("BubbleCircle::OnStart");

	GetOwner()->GetTransform()->SetLocalPosition(0, 0);
	GetOwner()->GetTransform()->SetLocalScale(0.5, 0.5);
}

void BubbleCircle::Update(float deltaTime)
{
	if (m_currSize <= 0.4)
		return;

	m_currSize -= deltaTime;
	GetOwner()->GetTransform()->SetLocalScale(m_currSize, m_currSize);
	auto& input = Platform::InputSystem::Instance();

	if (input.GetMousePressed(0))
	{
		//if ()
	};
}

void BubbleCircle::OnDisable()
{
}

void BubbleCircle::OnDestory()
{
}

Object* BubbleCircle::Clone()
{
	BubbleCircle* clone = new BubbleCircle(*this);
	InitClone(clone);
	return clone;
}

void BubbleCircle::Render(ID2D1DeviceContext7* ctx)
{

}