#include "pch.h"
#include "HitEffect.h"

#include "Animator.h"
#include "GameObject.h"

void HitEffect::Update(float deltaTime)
{
    auto* anim = GetOwner()->GetComponent<Animator>();
    if (anim && !anim->IsPlaying())
        Destroy(GetOwner());
}

Object* HitEffect::Clone()
{
    HitEffect* clone = new HitEffect(*this);
    InitClone(clone);
    return clone;
}
