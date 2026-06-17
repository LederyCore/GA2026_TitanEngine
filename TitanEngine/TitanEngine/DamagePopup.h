#pragma once
#include <map>

#include "Component.h"
#include "Texture2D.h"

using namespace TitanEngine;

class DamagePopup : public Component, public IUpdateable
{
public:
	void OnAwake() override;
	void OnEnable() override;
	void OnStart() override;
	void Update(float deltaTime) override;
	void OnDisable() override;
	void OnDestory() override;

	Object* Clone() override;

	void Init(std::string s);

	std::map<char, std::shared_ptr<Texture2D>> numbers;

	std::string s;

	float deleteTime = 2.0f;
};

