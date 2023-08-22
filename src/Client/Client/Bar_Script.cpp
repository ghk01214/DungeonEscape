#include "pch.h"
#include "Bar_Script.h"

#include <Engine.h>
#include <Transform.h>

Bar_Script::Bar_Script()
{
}

Bar_Script::~Bar_Script()
{
}

void Bar_Script::Awake()
{
	__super::Awake();
}

void Bar_Script::Start()
{
	__super::Start();

	m_pos = GetTransform()->GetLocalPosition();
	m_scale = GetTransform()->GetLocalScale();

	float width{ static_cast<float>(GEngine->GetWindow().width) };
	float height{ static_cast<float>(GEngine->GetWindow().height) };

	m_scaleRatio = { m_scale.x / width, m_scale.y / height };
}

void Bar_Script::Update()
{
	__super::Update();

	float width{ static_cast<float>(GEngine->GetWindow().width) };
	float height{ static_cast<float>(GEngine->GetWindow().height) };

	m_pos = GetTransform()->GetLocalPosition();

	m_scale.x = m_scaleRatio.x * width;
	m_scale.y = m_scaleRatio.y * height;
}

void Bar_Script::LateUpdate()
{
	__super::LateUpdate();
}
