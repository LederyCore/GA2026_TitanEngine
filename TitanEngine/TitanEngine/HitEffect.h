#pragma once
#include "Animator.h"
#include "Component.h"

using namespace TitanEngine;

class HitEffect : public Component, public IUpdateable
{
public:
    void Update(float deltaTime) override;
    Object* Clone() override;
};
