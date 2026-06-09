#include "pch.h"
#include "TestComponent.h"

void TitanEngine::TestComponent::OnStart()
{

}

TitanEngine::Object* TitanEngine::TestComponent::Clone()
{
	TestComponent* clone = new TestComponent(*this);
	InitClone(clone);
	return clone;
}