#pragma once
#include "SystemLocator.h"

namespace TitanEngine
{
    class PhysicsSystem;

    class IPhysics
    {
    public:
        virtual ~IPhysics();
        virtual void OnPhysicsUpdate(float fixedTime) = 0;
        virtual bool IsPhysicsActive() const = 0;
    };
}