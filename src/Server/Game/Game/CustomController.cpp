#include "pch.h"
#include "CustomController.h"
#include "CollisionPairInfo.h"
#include "PhysDevice.h"
#include "RigidBody.h"
#include "CapsuleCollider.h"
#include "PhysicsRay.h"
#include "RaycastHit.h"
#include "PhysQuery.h"
#include "GameObject.h"
#include "Transform.h"

#include "MessageHandler.h"

using namespace physx;

CustomController::CustomController(GameObject* ownerGameObject, Component* ownerComponent)
	: Component(ownerGameObject, ownerComponent)
{
}

CustomController::~CustomController()
{
}

void CustomController::Init()
{
	//멤버 component 생성
	Transform* t = m_ownerGameObject->GetTransform();
	m_body = AddComponent<RigidBody>();
	//m_collider = m_body->AddCollider<CapsuleCollider>(m_ownerGameObject->GetTransform()->GetScale());
	m_collider = m_body->AddComponent<CapsuleCollider>(m_body, m_ownerGameObject->GetTransform()->GetScale());

		//body 초기화
	m_body->SetSleepThresholder(1e-6f);
	m_body->SetRotation(90, PhysicsAxis::Z);				//capsule is laying by default.
	m_body->SetRotationLockAxis(PhysicsAxis::All, true);
	m_body->SetSleepThresholder(0.001f);

	m_body->SetKinematic(false);

	// 사용할 키 종류를 추가
	// enum KEY_ORDER 순서에 맞게
	m_useKeyType.clear();
	m_useKeyType.push_back(server::KEY_TYPE::W);
	m_useKeyType.push_back(server::KEY_TYPE::S);
	m_useKeyType.push_back(server::KEY_TYPE::A);
	m_useKeyType.push_back(server::KEY_TYPE::D);
	m_useKeyType.push_back(server::KEY_TYPE::SPACE);

	m_keyboardLeft = false;
	m_keyboardRight = false;
	m_keyboardUp = false;
	m_keyboardDown = false;
	m_keyboardSpace = false;

	m_startJump = false;
}

void CustomController::Release()
{
	m_collider = nullptr;
	SafeRelease(m_body);
}

void CustomController::Move()
{
	DirectionInput();
	Movement();
}

bool CustomController::CheckOnGround(CollisionInfoType type, PxVec3& surfaceNormal)
{
	m_onGround = false;

	PxVec3 normal;
	PxVec3 up(0.f, 1.f, 0.f);

	const auto info = m_collider->GetCollisionInfo(type);
	int infoCnt = info.size();
	if (infoCnt > 0)
	{
		for (int i = 0; i < infoCnt; ++i)
		{
			for (int j = 0; j < info[i]->GetContactCount(); ++j)
			{
				normal = info[i]->GetContact(j).normal;
				PxVec3 horizontalNormal = PxVec3(normal.x, 0, normal.z).getNormalized();

				float dotProduct = up.dot(normal);
				dotProduct = max(-1.0f, min(1.0f, dotProduct)); // clamp value by -1~1

				float angleRadians = acos(dotProduct);
				float angleDegrees = angleRadians * (180.0f / PxPi);
				if (angleDegrees < m_degreeThreshold)
				{
					//floor, slopes
					surfaceNormal = normal;
					m_onGround = true;
					return true;
				}
				else
				{
					//walls
				}
			}
		}
	}

	return false;
}

void CustomController::GetSlidingVector(CollisionInfoType type)
{
	PxVec3 dir = m_moveDirection;
	PxVec3 up(0.f, 1.f, 0.f);
	float radianThreshold = m_degreeThreshold * (PxPi / 180);

	const auto info = m_collider->GetCollisionInfo(type);
	int infoCnt = info.size();

	m_slidingVector = PxVec3(0.f, 0.f, 0.f);

	if (infoCnt > 0)
	{
		for (int i = 0; i < infoCnt; ++i)
		{
			for (int j = 0; j < info[i]->GetContactCount(); ++j)
			{
				Contact cont = info[i]->GetContact(j);
				if (acos(cont.normal.dot(up)) <= radianThreshold)
				{
					// ignore floor/slope contacts
					continue;
				}

				// calculate the direction from the contact point to the character's center
				PxVec3 contactToCenter = m_body->GetPosition() - cont.point;
				contactToCenter.normalize();

				// check if the character is moving away from the wall
				float contactDotProduct = contactToCenter.dot(dir);
				if (contactDotProduct > 0.0f)
				{
					// moving away from the wall, no sliding vector should be applied
					continue;
				}

				// calculate projection of the movement direction onto the surface normal
				PxVec3 projection = cont.normal * cont.normal.dot(dir);

				// calculate the sliding vector
				PxVec3 slidingVector = dir - projection;

				// accumulate the sliding vector
				m_slidingVector += slidingVector;
			}
		}
	}

	if (m_slidingVector.magnitudeSquared() > 0.0f)
	{
		m_slidingVector.normalize();
	}
}

bool CustomController::CheckOnGround_Raycast()
{
	auto device = PhysDevice::GetInstance();
	auto query = device->GetQuery();

	RaycastHit hit;
	PhysicsRay ray;
	ray.direction = PxVec3(0.f, -1.f, 0.f);
	ray.distance = 0.21f;			//45degree-raycast distance : 0.2
	ray.point = m_body->GetPosition()
		- PxVec3(0.f, (m_collider->GetRadius() + m_collider->GetHalfHeight()), 0.f);

	if (query->Raycast(hit, ray, 1 << (uint8_t)PhysicsLayers::Map, PhysicsQueryType::All, m_body))
	{
		return true;
	}

	return false;
}

void CustomController::DirectionInput()
{
	bool serverConnected = true;

	if (serverConnected)
	{
		m_moveDirection = PxVec3(0.f, 0.f, 0.f);

		PxVec3 up = PxVec3(0.f, 1.f, 0.f);
		PxVec3 right = up.cross(m_cameraLook);

		if (m_keyboardLeft)
			m_moveDirection -= right;
		else if (m_keyboardRight)
			m_moveDirection += right;

		if (m_keyboardUp)
			m_moveDirection += m_cameraLook;
		else if (m_keyboardDown)
			m_moveDirection -= m_cameraLook;

		m_moveDirection.normalize();
	}

	else
	{
		//서버 연결 안할거면 카메라 정보도 없다
		m_moveDirection = PxVec3(0.f, 0.f, 0.f);

		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
			m_moveDirection.x = -1.f;
		else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
			m_moveDirection.x = 1.f;

		if (GetAsyncKeyState(VK_UP) & 0x8000)
			m_moveDirection.z = -1.f;
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
			m_moveDirection.z = 1.f;

		m_moveDirection.normalize();
	}
}

void CustomController::Movement()
{
	if (m_body->isKinematic())
		return;

	//check onGround

	GetSlidingVector(CollisionInfoType::Stay);
	PxVec3 surfaceNormal{ 0.f };
	CheckOnGround(CollisionInfoType::Stay, surfaceNormal);

	//if ((GetAsyncKeyState(VK_SPACE) & 0x8000) && m_onGround)
	if (m_keyboardSpace && m_onGround)
	{
		PxVec3 up{ 0.f, 1.f, 0.f };
		m_body->GetPosition() += up * 0.05f;

		//choice 1 : add velocity
		PxVec3 velocity = m_body->GetVelocity();
		velocity.y = m_jumpSpeed;
		m_body->SetVelocity(velocity);

		#pragma region choice2 add force
		//PxVec3 force = m_moveDirection * m_body->GetMass() * deltaTime * adjustmentValue;
		//m_body->GetBody()->addForce(force, PxForceMode::eIMPULSE);
		#pragma endregion

		m_onGround = false;
		m_startJump = true;
	}

	//friction adjustment
	if (!m_onGround)
		m_collider->SetFriction(0.f);
	else
		m_collider->SetFriction(1.f);

	// Apply the adjusted vector to the character's velocity
	if (m_slidingVector.magnitude() > 0)
	{
		m_slidingVector *= m_moveSpeed;
		m_slidingVector.y = m_body->GetVelocity().y;
		m_body->SetVelocity(m_slidingVector);	// * movespeed?
	}
	else
	{
		m_moveDirection *= m_moveSpeed;
		m_moveDirection.y = m_body->GetVelocity().y;
		m_body->SetVelocity(m_moveDirection);
	}

	m_slidingVector = PxVec3(0.f);
	m_moveDirection = PxVec3(0.f);
	KeyboardClear();
}

void CustomController::ClearControllerCollisionInfo()
{
	m_body->ClearCollidersCollisionInfo();
}

void CustomController::CameraLookReceive(Vec3& CameraLook)
{
	m_cameraLook.x = CameraLook.x;
	m_cameraLook.y = CameraLook.y;
	m_cameraLook.z = CameraLook.z;
}

void CustomController::KeyboardReceive(ulong32_t key)
{
	for (int32_t i = 0; i < m_useKeyType.size(); ++i)
	{
		int32_t temp{ 3 };

		temp <<= (i * 2);
		temp &= key;
		temp >>= (i * 2);

		switch (magic_enum::enum_cast<server::KEY_STATE>(temp).value())
		{
			case server::KEY_STATE::NONE:
			{
				if (i == W)
				{
					//std::cout << "UP::None";
				}
				if (i == S)
				{
					//std::cout << "DOWN::None";
				}
				if (i == A)
				{
					//std::cout << "LEFT::None";
				}
				if (i == D)
				{
					//std::cout << "RIGHT::None";
				}
				if (i == SPACE)
				{
					//std::cout << "SPACE::None";
				}
			}
			break;
			case server::KEY_STATE::PRESS:
			{
				if (i == W)
				{
					m_keyboardUp = true;
				}
				if (i == S)
				{
					m_keyboardDown = true;
				}
				if (i == A)
				{
					m_keyboardLeft = true;
				}
				if (i == D)
				{
					m_keyboardRight = true;
				}
				if (i == SPACE)
				{
					m_keyboardSpace = false;
				}
			}
			break;
			case server::KEY_STATE::DOWN:
			{
				if (i == W)
				{
					m_keyboardUp = true;
				}
				if (i == S)
				{
					m_keyboardDown = true;
				}
				if (i == A)
				{
					m_keyboardLeft = true;
				}
				if (i == D)
				{
					m_keyboardRight = true;
				}
				if (i == SPACE)
				{
					m_keyboardSpace = true;
				}
			}
			break;
			case server::KEY_STATE::UP:
			{
				if (i == W)
				{
					m_keyboardUp = false;
				}
				if (i == S)
				{
					m_keyboardDown = false;
				}
				if (i == A)
				{
					m_keyboardLeft = false;
				}
				if (i == D)
				{
					m_keyboardRight = false;
				}
				if (i == SPACE)
				{
					m_keyboardSpace = false;
				}
			}
			break;
			default:
			break;
		}
	}
}

void CustomController::KeyboardClear()
{
	m_keyboardLeft	= false;
	m_keyboardRight = false;
	m_keyboardUp	= false;
	m_keyboardDown	= false;
	m_keyboardSpace	= false;
}

void CustomController::SetMoveSpeed(float value)
{
	m_moveSpeed = value;
}

float CustomController::GetMoveSpeed()
{
	return m_moveSpeed;
}

void CustomController::SetJumpSpeed(float value)
{
	m_jumpSpeed = value;
}

float CustomController::GetJumpSpeed()
{
	return m_moveSpeed;
}

RigidBody* CustomController::GetBody()
{
	return m_body;
}

CapsuleCollider* CustomController::GetCollider()
{
	return m_collider;
}

bool CustomController::IsOnGround()
{
	return m_onGround;
}

bool CustomController::IsStartJump()
{
	if (m_startJump == true)
	{
		m_startJump = false;
		return true;
	}

	return false;
}
