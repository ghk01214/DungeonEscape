#include "pch.h"
#include "TestCameraScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"

CTestCameraScript::CTestCameraScript()
{
}

CTestCameraScript::~CTestCameraScript()
{
}

void CTestCameraScript::LateUpdate()
{
	// 입력 처리
	KeyInput();
}

void CTestCameraScript::KeyInput(void)
{
	// 카메라의 위치값을 가져옴
	Vec3 pos = GetTransform()->GetLocalPosition();

	std::shared_ptr<CTransform> Transform = std::static_pointer_cast<CTransform>(GetTransform());

	if (INPUT->GetButton(KEY_TYPE::W))
		pos += GetTransform()->GetLook() * m_speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::S))
		pos -= GetTransform()->GetLook() * m_speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::A))
		pos -= GetTransform()->GetRight() * m_speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::D))
		pos += GetTransform()->GetRight() * m_speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::Q))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.x += DELTA_TIME * 0.5f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if (INPUT->GetButton(KEY_TYPE::E))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.x -= DELTA_TIME * 0.5f;
		GetTransform()->SetLocalRotation(rotation);
	}

	GetTransform()->SetLocalPosition(pos);
}
