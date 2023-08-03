#include "pch.h"
#include "Camera_Script.h"

#include <Transform.h>
#include <Camera.h>
#include <GameObject.h>
#include <Input.h>
#include <Timer.h>
#include <SceneManager.h>
#include <Scene.h>

Camera_Basic::Camera_Basic() :
	m_speed{ 2000.f },
	m_lengthX{ 1000.f },
	m_lengthY{ 600.f },
	m_rotationAxisX{ 0.f },
	m_rotationAxisY{ 0.f },
	m_distance{ 1.f }
{
}

Camera_Basic::~Camera_Basic()
{
}

void Camera_Basic::Update(void)
{
}

void Camera_Basic::LateUpdate()
{
	std::shared_ptr<CScene> activeScene = GET_SINGLE(SceneManager)->GetActiveScene();
	auto& player = activeScene->GetPlayer();

	// 플레이어가 존재하지 않을 경우
	if (player.empty())
	{
		Vec3 pos = GetTransform()->GetWorldPosition();

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
			rotation.x += DELTA_TIME * 90.f;
			GetTransform()->SetLocalRotation(rotation);
		}

		if (INPUT->GetButton(KEY_TYPE::E))
		{
			Vec3 rotation = GetTransform()->GetLocalRotation();
			rotation.x -= DELTA_TIME * 90.f;
			GetTransform()->SetLocalRotation(rotation);
		}

		if (INPUT->GetButton(KEY_TYPE::Z))
		{
			Vec3 rotation = GetTransform()->GetLocalRotation();
			rotation.y += DELTA_TIME * 90.f;
			GetTransform()->SetLocalRotation(rotation);
		}

		if (INPUT->GetButton(KEY_TYPE::C))
		{
			Vec3 rotation = GetTransform()->GetLocalRotation();
			rotation.y -= DELTA_TIME * 90.f;
			GetTransform()->SetLocalRotation(rotation);
		}

		//if (INPUT->GetButtonDown(KEY_TYPE::RBUTTON))
		//{
		//	const POINT& pos = INPUT->GetMousePos();
		//	GET_SINGLE(SceneManager)->Pick(pos.x, pos.y);
		//}

		GetTransform()->SetLocalPosition(pos);
	}
	// 플레이어가 존재할 경우
	else
	{
		const shared_ptr<Transform>& playerTransform = (*player.begin())->GetTransform();
		auto transform = GetTransform();

		// 크기
		transform->SetLocalScale(Vec3(1.f, 1.f, 1.f));
		// 자전
		transform->SetLocalRotation(Vec3(12.f, 0.f, 0.f));
		// 이동
		Vec2 movement{ m_lengthX * m_distance, m_lengthY * m_distance };
		transform->SetLocalPosition(Vec3(0.f, movement.y + 40.f, -movement.x));
		// 공전
		Matrix matWorld = Matrix::CreateRotationX(XMConvertToRadians(m_rotationAxisX)) * Matrix::CreateRotationY(XMConvertToRadians(m_rotationAxisY)) * Matrix::CreateTranslation(playerTransform->GetWorldMatrix().Translation());
		transform->SetWorldMatrix(matWorld);
		// 부모
		//transform->SetParent(playerTransform);

		// X축 회전
		{
			long MouseMoveY = GET_SINGLE(CInput)->Get_DIMMoveState(CInput::DIMM_Y);
			m_rotationAxisX += MouseMoveY * DELTA_TIME * 5.f;

			if (m_rotationAxisX < -30.f)
				m_rotationAxisX = -30.f;
			else if (m_rotationAxisX > 30.f)
				m_rotationAxisX = 30.f;
		}
		// Y축 회전
		{
			long MouseMoveX = GET_SINGLE(CInput)->Get_DIMMoveState(CInput::DIMM_X);
			m_rotationAxisY += MouseMoveX * DELTA_TIME * 5.f;
		}
		// 마우스 휠 이동
		{
			m_distance -= GET_SINGLE(CInput)->Get_DIMMoveState(CInput::DIMM_WHEEL) * 0.0003f;	// 1틱 -> 120

			if (m_distance < 0.4)
				m_distance = 0.4;

			else if (m_distance > 1.2f)
				m_distance = 1.2f;
		}

	}
}