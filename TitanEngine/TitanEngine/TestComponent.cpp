#include "pch.h"
#include "TestComponent.h"
#include "DebugConsole/DebugConsole.h"

void TitanEngine::TestComponent::OnAwake()
{
	LOG_DEBUG("테스트 컴포넌트 Awake");
}

void TitanEngine::TestComponent::OnEnable()
{
	LOG_DEBUG("테스트 컴포넌트 Enable");
}

void TitanEngine::TestComponent::OnStart()
{
	LOG_DEBUG("테스트 컴포넌트 Start");
}

void TitanEngine::TestComponent::Update(float deltaTime)
{
	//LOG_DEBUG("테스트 컴포넌트 Update");
}

void TitanEngine::TestComponent::OnDisable()
{
	LOG_DEBUG("테스트 컴포넌트 OnDisable");
}

void TitanEngine::TestComponent::OnDestory()
{
	LOG_DEBUG("테스트 컴포넌트 OnDestroy");
}

TitanEngine::Object* TitanEngine::TestComponent::Clone()
{
	TestComponent* clone = new TestComponent(*this);
	InitClone(clone);
	return clone;
}