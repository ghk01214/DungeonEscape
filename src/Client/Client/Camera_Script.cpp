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
	m_lengthX{ 400.f },
	m_lengthY{ 200.f },
	m_rotationAxisX{ 0.f },
	m_rotationAxisY{ 180.f },
	m_distanceRatio{ 1.f }
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
	// 시네마틱 카메라가 작동할 경우 멈춘다.
	if (m_bPlayCinematic)
		return;

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

#pragma region [주석]
	//else
	//{
	//	const shared_ptr<Transform>& playerTransform = (*player.begin())->GetTransform();
	//	auto transform = GetTransform();

	//	//// 크기
	//	//transform->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	//	//// 자전
	//	//transform->SetLocalRotation(Vec3(12.f, 0.f, 0.f));
	//	//// 이동
	//	Vec2 movement{ m_lengthX * m_distance, m_lengthY * m_distance };
	//	//transform->SetLocalPosition(Vec3(0.f, movement.y + 40.f, -movement.x));

	//	Matrix matRotation = Matrix::CreateRotationX(XMConvertToRadians(30.f));
	//	matRotation *= Matrix::CreateRotationY(XMConvertToRadians(0.f));
	//	matRotation *= Matrix::CreateRotationZ(XMConvertToRadians(0.f));
	//	Matrix matTranslation = Matrix::CreateTranslation(Vec3(0.f, movement.y, -movement.x));

	//	// 공전
	//	Matrix matWorld = Matrix::CreateTranslation(Vec3(0.f, -200.f, 0.f)) * matRotation * matTranslation * Matrix::CreateRotationX(XMConvertToRadians(m_rotationAxisX)) * Matrix::CreateRotationY(XMConvertToRadians(m_rotationAxisY)) * Matrix::CreateTranslation(playerTransform->GetWorldMatrix().Translation());
	//	transform->SetWorldMatrix(matWorld);

	//	// 부모
	//	//transform->SetParent(playerTransform);

	//	// X축 회전
	//	{
	//		long MouseMoveY = GET_SINGLE(CInput)->Get_DIMMoveState(CInput::DIMM_Y);
	//		m_rotationAxisX += MouseMoveY * DELTA_TIME * 5.f;

	//		if (m_rotationAxisX < -20.f)
	//			m_rotationAxisX = -20.f;
	//		else if (m_rotationAxisX > 40.f)
	//			m_rotationAxisX = 40.f;
	//	}
	//	// Y축 회전
	//	{
	//		long MouseMoveX = GET_SINGLE(CInput)->Get_DIMMoveState(CInput::DIMM_X);
	//		m_rotationAxisY += MouseMoveX * DELTA_TIME * 5.f;
	//	}
	//	// 마우스 휠 이동
	//	{
	//		m_distance -= GET_SINGLE(CInput)->Get_DIMMoveState(CInput::DIMM_WHEEL) * 0.0003f;	// 1틱 -> 120

	//		if (m_distance < 0.1)
	//			m_distance = 0.1;

	//		else if (m_distance > 0.7f)
	//			m_distance = 0.7f;
	//	}

	//	// 카메라의 위치
	//	//Vec3 vCameraPos = transform->GetWorldPosition();

	//	//// 플레이어의 위치
	//	//Vec3 vPlayerPos = playerTransform->GetWorldPosition();

	//	//m_distanceBetweenPlayerAndCamera = SimpleMath::Vector3::Distance(vCameraPos, vPlayerPos);

	//	//Print(m_distanceBetweenPlayerAndCamera);
	//}
#pragma endregion

	// 플레이어가 존재할 경우
	else
	{
		const shared_ptr<Transform>& playerTransform = (*player.begin())->GetTransform();
		auto transform = GetTransform();

		float characterHeight = 300.f * 0.5f;	// 캐릭터의 높이 300.f, 캡슐의 중점을 구하기 위해서 x 0.5f


		// 회전값을 사용하여 look만 구함
		Matrix matLook = Matrix::CreateRotationX(XMConvertToRadians(m_rotationAxisX)) * Matrix::CreateRotationY(XMConvertToRadians(m_rotationAxisY));

		// 회전값을 적용하여 구한 camera Look
		Vec3 cameraLook = matLook.Backward();
		cameraLook.Normalize();

		// 서버에서 raycasting 되는 시작 위치를 구함
		Vec3 vPlayerPos = playerTransform->GetWorldMatrix().Translation();
		vPlayerPos.y += characterHeight;



		// 플레이어와 물체가 충돌되는 거리인 raycastDistance가 카메라의 최대 거리인 cameraDistance보다 클 경우, cameraDistance 적용.
		// 반대일 경우 raycastDistance 적용

		// client용 카메라 거리 같은 경우 distanceRatio에 영향을 받아 마우스로 조정이 가능함.
		float cameraDistance = m_cameraDistance * m_distanceRatio;

		float raycastDistance = GetCamera()->GetRaycastDistance();

		//if (GET_SINGLE(CInput)->Button_Pressing(CInput::DIMB_LBUTTON))
			//Print(raycastDistance);

		raycastDistance *= 0.95f;
		Vec3 vCrashPoint{};
		float distance{};

		if (raycastDistance < 0)
		{
			// 서버에서 충돌 되는 지점을 카메라 위치로 구함
			distance = cameraDistance;
		}
		else
		{
			// client에서 적용하는 거리로 카메라 위치를 구함
			distance = raycastDistance;
		}

		vCrashPoint = vPlayerPos + cameraLook * distance;

		// 여기서 충돌되는 지점이 eye 임. 이제 up을 구함

		// 1. look과 기본 up(0, 1, 0)을 외적하여 right를 구함
		Vec3 vRight = cameraLook.Cross(Vec3(0.f, 1.f, 0.f));

		// 2. right와 look을 외적하여 up을 구함
		Vec3 vUp = vRight.Cross(cameraLook);


		// 구한 eye, at, up을 사용하여 행렬을 구성
		Matrix matWorld = Matrix::CreateWorld(vCrashPoint, cameraLook, vUp);
		float basicHeight = 1000.f * (1.f - m_distanceRatio);		// basicHeight -> 수정
		matWorld._42 -= basicHeight;

		float BasicDistanceFromPlayer = 600.f;						// BasicDistanceFromPlayer -> 수정
		Vec3 vBasicDistance = cameraLook * BasicDistanceFromPlayer;
		matWorld._41 -= vBasicDistance.x;
		matWorld._43 -= vBasicDistance.z;

		// 구성한 행렬값을 설정
		transform->SetWorldMatrix(matWorld);


		// X축 회전
		{
			// 마우스 왼쪽 버튼을 누르고 있을 경우
			//if (GET_SINGLE(CInput)->Button_Pressing(CInput::DIMB_LBUTTON))
			{
				long MouseMoveY = GET_SINGLE(CInput)->Get_DIMMoveState(CInput::DIMM_Y);
				m_rotationAxisX += MouseMoveY * DELTA_TIME * 5.f;

				if (m_rotationAxisX < -30.f)
					m_rotationAxisX = -30.f;
				else if (m_rotationAxisX > 0.f)
					m_rotationAxisX = 0.f;
			}
		}
		// Y축 회전
		{
			long MouseMoveX = GET_SINGLE(CInput)->Get_DIMMoveState(CInput::DIMM_X);
			m_rotationAxisY += MouseMoveX * DELTA_TIME * 5.f;
		}

		// 마우스 휠 이동
		{
			m_distanceRatio -= GET_SINGLE(CInput)->Get_DIMMoveState(CInput::DIMM_WHEEL) * 0.0003f;	// 1틱 -> 120

			if (m_distanceRatio < 0.3)
				m_distanceRatio = 0.3;

			else if (m_distanceRatio > 0.8f)
				m_distanceRatio = 0.8f;
		}

	}
}