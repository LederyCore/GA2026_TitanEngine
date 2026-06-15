#include "pch.h"
#include "Bubble.h"

#include <random>
#include <DebugConsole/DebugConsole.h>

#include "GameObject.h"

void Bubble::OnAwake()
{
}

void Bubble::OnEnable()
{
}

void Bubble::OnStart()
{
	std::random_device rd;
	std::default_random_engine generator(rd());
	std::uniform_int_distribution distribution(-300, 300);

	LOG_DEBUG("Bubble::OnStart : %d", distribution(generator));
	GetOwner()->GetTransform()->SetLocalPosition(distribution(generator), distribution(generator));

}

void Bubble::Update(float deltaTime)
{
}

void Bubble::OnDisable()
{
}

void Bubble::OnDestory()
{
}

Object* Bubble::Clone()
{
	Bubble* clone = new Bubble();
	InitClone(clone);
	return clone;
}

void Bubble::Render(ID2D1DeviceContext7* ctx)
{
}
