#include "pch.h"
#include "UI_Script.h"
#include "Button_Script.h"

#include "Transform.h"
#include "Input.h"
#include "Engine.h"

Button_Script::Button_Script() :
	m_click{ false },
	m_input{ nullptr }
{
}

Button_Script::~Button_Script()
{
}

void Button_Script::Awake()
{
	__super::Awake();
}

void Button_Script::Start()
{
	__super::Start();

	m_input = GET_SINGLE(CInput)->GetInstance();
	m_pos = GetTransform()->GetLocalPosition();
	m_scale = GetTransform()->GetLocalScale();

	float width{ static_cast<float>(GEngine->GetWindow().width) };
	float height{ static_cast<float>(GEngine->GetWindow().height) };

	m_scaleRatio = { m_scale.x / width, m_scale.y / height };
}

void Button_Script::Update()
{
	__super::Update();

	m_mousePos = m_input->GetMousePos();

	float width{ static_cast<float>(GEngine->GetWindow().width) };
	float height{ static_cast<float>(GEngine->GetWindow().height) };

	m_mousePos.x -= width / 2;
	m_mousePos.y -= height / 2;
	m_mousePos.y = -m_mousePos.y;

	m_pos.y = -(height / 4);
	m_scale.x = m_scaleRatio.x * width;
	m_scale.y = m_scaleRatio.y * height;
}

void Button_Script::LateUpdate()
{
	__super::LateUpdate();
}
