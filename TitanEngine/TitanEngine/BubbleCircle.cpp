#include "pch.h"
#include "BubbleCircle.h"

#include <DebugConsole/DebugConsole.h>

#include "Bubble.h"
#include "GameObject.h"
#include "Transform.h"


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

	//m_Timer = GetOwner()->GetTransform()->GetParent()->GetOwner()->GetComponent<Bubble>()->m_Timer;
}

void BubbleCircle::Update(float deltaTime)
{
	if (m_currSize <= 0.0f)
	{
		// 시간 초과 - 부모 Bubble GameObject 파괴
		Transform* parent = GetOwner()->GetTransform()->GetParent();
		if (parent)
			Destroy(parent->GetOwner());
		return;
	}

	if (m_Timer->m_CurTime <= 0.0f || m_Timer->IsGameClear())
		return;

	m_currSize -= deltaTime * 2;
	m_currSize = std::max(m_currSize, 0.0f);

	GetOwner()->GetTransform()->SetLocalScale(m_currSize, m_currSize);
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