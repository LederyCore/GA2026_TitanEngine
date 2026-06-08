#include "pch.h"
#include "IPhysics.h"
#include "PhysicsSystem.h"

TitanEngine::IPhysics::~IPhysics()
{
    // 소멸 시 자동 해제
    auto* ps = SystemLocator::GetPhysicsSystem();
    if (ps) ps->Unregister (this);
}
