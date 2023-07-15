#include "pch.h"
#include "TestCameraScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"

TestCameraScript::TestCameraScript()
{
}

TestCameraScript::~TestCameraScript()
{
}

void TestCameraScript::LateUpdate()
{
	Vec3 pos = GetTransform()->GetLocalPosition();
	Vec3 rotation = GetTransform()->GetLocalRotation();

	if (INPUT->GetButton(KEY_TYPE::W))
		pos += GetTransform()->GetLook() * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::S))
		pos -= GetTransform()->GetLook() * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::A))
		pos -= GetTransform()->GetRight() * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::D))
		pos += GetTransform()->GetRight() * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::Q))
	{
		rotation.x += DELTA_TIME * 90.f;
	}

	if (INPUT->GetButton(KEY_TYPE::E))
	{
		rotation.x -= DELTA_TIME * 90.f;
	}

	if (INPUT->GetButton(KEY_TYPE::Z))
	{
		rotation.y += DELTA_TIME * 90.f;
	}

	if (INPUT->GetButton(KEY_TYPE::C))
	{
		rotation.y -= DELTA_TIME * 90.f;
	}

	//if (INPUT->GetButtonDown(KEY_TYPE::RBUTTON))
	//{
	//	const POINT& pos = INPUT->GetMousePos();
	//	GET_SINGLE(SceneManager)->Pick(pos.x, pos.y);
	//}

	GetTransform()->SetLocalPosition(pos);
	GetTransform()->SetLocalRotation(rotation);
}