#include "pch.h"
#include "DamagePopup.h"

#include <DebugConsole/DebugConsole.h>

#include "GameObject.h"
#include "Scene.h"
#include "SpriteRenderer.h"

void DamagePopup::OnAwake()
{
}

void DamagePopup::OnEnable()
{
}

void DamagePopup::OnStart()
{
}

void DamagePopup::Update(float deltaTime)
{
	auto p = GetOwner()->GetTransform()->GetLocalPosition();
	p.y -= deltaTime * 50;

	GetOwner()->GetTransform()->SetLocalPosition(p);

	if (deleteTime >= 0)
	{
		deleteTime -= deltaTime;

		for (int i=0; i<gos.size(); i++)
		{
			gos[i]->GetComponent<SpriteRenderer>()->sprite.tint.a -= deltaTime;
		}
	}
	else
	{
		Destroy(GetOwner());
	}
}

void DamagePopup::OnDisable()
{
}

void DamagePopup::OnDestory()
{
}

Object* DamagePopup::Clone()
{
	return nullptr;
}

void DamagePopup::Init(std::string s)
{
	for (int i = 0; i < s.size(); i++)
	{
		if (numbers.find(s[i]) == numbers.end())
			continue;

		GameObject* go = GetScene()->AddObject("Damage");
		go->AddComponent<SpriteRenderer>();
		go->GetComponent<SpriteRenderer>()->sprite.texture = numbers[s[i]];
		Vector2 p = { -50,0 };
		p.x += (float)i * 20;

		go->GetTransform()->SetParent(GetOwner()->GetTransform());
		go->GetTransform()->SetLocalPosition(p);

		gos.push_back(go);
	}
}

