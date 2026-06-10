#include "pch.h"
#include "TestComponent.h"
#include "GameObject.h"
#include "InputSystem/InputSystem.h"
#include "DebugConsole/DebugConsole.h"
#include <wrl/client.h>
#include <d2d1.h>

using Microsoft::WRL::ComPtr;
using namespace DirectX::SimpleMath;

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
	GameObject* owner = GetOwner();
	if (owner == nullptr) return;
	Transform* t = owner->GetTransform();
	if (t == nullptr) return;

	Vector2 pos = t->GetLocalPosition();  // ← D2D1_POINT_2F → Vector2

	float dx = 0.0f;
	float dy = 0.0f;

	auto& input = Platform::InputSystem::Instance();

	if (input.GetKeyDown(VK_LEFT))  dx -= m_speed * deltaTime;
	if (input.GetKeyDown(VK_RIGHT)) dx += m_speed * deltaTime;
	if (input.GetKeyDown(VK_UP))    dy -= m_speed * deltaTime;
	if (input.GetKeyDown(VK_DOWN))  dy += m_speed * deltaTime;
	if (input.GetKeyPressed(VK_SPACE))
	{
		LOG_DEBUG("삭제하라");
		Destroy(GetOwner());
	}

	if (dx != 0.0f || dy != 0.0f)
		t->SetLocalPosition(pos.x + dx, pos.y + dy);
}
void TitanEngine::TestComponent::OnDisable()
{
	LOG_DEBUG("테스트 컴포넌트 OnDisable");
}

void TitanEngine::TestComponent::OnDestory()
{
	LOG_DEBUG("테스트 컴포넌트 OnDestroy");
}

void TitanEngine::TestComponent::Render(ID2D1DeviceContext7* ctx)
{
	if (ctx == nullptr) return;
	GameObject* owner = GetOwner();
	if (owner == nullptr) return;
	Transform* t = owner->GetTransform();
	if (t == nullptr) return;

	Vector2 worldPos = t->GetWorldPosition();

	ComPtr<ID2D1SolidColorBrush> brush;
	ctx->CreateSolidColorBrush(m_color, brush.GetAddressOf());

	// Vector2 → D2D1 변환
	D2D1_ELLIPSE ellipse = D2D1::Ellipse(
		D2D1::Point2F(worldPos.x, worldPos.y),
		m_radius, m_radius
	);
	ctx->FillEllipse(ellipse, brush.Get());
}

TitanEngine::Object* TitanEngine::TestComponent::Clone()
{
	TestComponent* clone = new TestComponent(*this);
	InitClone(clone);
	return clone;
}
