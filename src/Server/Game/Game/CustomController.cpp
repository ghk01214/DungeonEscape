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

CustomController::CustomController(GameObject* ownerGameObject, Component* ownerComponent, bool player)
	: Component(ownerGameObject, ownerComponent), m_isPlayer(player)
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

	m_keyboardInput.clear();
	m_keyboardInput.reserve(KEY_ORDER::MAX);
	for (int32_t i = 0; i < KEY_ORDER::MAX; ++i)
	{
		m_keyboardInput.push_back(KeyInput{});
	}

	m_cameraLook = PxVec3(0.f, 0.f, 1.f);
}

void CustomController::Release()
{
	m_collider = nullptr;
	SafeRelease(m_body);
}

void CustomController::PlayerMove()
{
	if (!m_ownerGameObject->AccessAuthorized())
		return;

	m_body->SetRigidBodySleep(false);

	if (m_isPlayer)
	{
		DirectionInput_Player();
		Movement_Player();
	}

	CheckOnGround_Raycast();
}

void CustomController::MonsterMove(physx::PxVec3 targetDir)
{
	if (!m_ownerGameObject->AccessAuthorized())
		return;

	m_body->SetRigidBodySleep(false);

	if (!m_isPlayer)
	{
		DirectionInput_Monster(targetDir);
		Movement_Monster();
	}

	CheckOnGround_Raycast();
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
					if (type == CollisionInfoType::Stay)
					{
						//floor, slopes
						surfaceNormal = normal;
						m_onGround = true;
					}

					if (type == CollisionInfoType::Enter)
					{
						m_BounceFromAttack = false;			//땅에 착지했으니 수평 SetVelocity를 다시 활성화 가능한 상태로
					}
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
	ray.distance = 1000.f;						//raycast 측정 거리
	ray.point = m_body->GetPosition()
		- PxVec3(0.f, (m_collider->GetRadius() + m_collider->GetHalfHeight() - 5.f), 0.f);

	if (query->Raycast(hit, ray, 1 << static_cast<uint8_t>(PhysicsLayers::MAP), PhysicsQueryType::All, m_body))
	{
		m_distanceFromGround = hit.distance;
		return true;
	}
	else
	{
		m_distanceFromGround = -1.f;				//raycast 실패 (밑에 아예 물체가 없거나 raycast 측정거리보다 멀다)
		return false;
	}
}

bool CustomController::RaycastGround(RaycastHit& hit)
{
	auto device = PhysDevice::GetInstance();
	auto query = device->GetQuery();


	PhysicsRay ray;
	ray.direction = PxVec3(0.f, -1.f, 0.f);
	ray.distance = 2000.f;						//raycast 측정 거리
	ray.point = m_body->GetPosition()
		- PxVec3(0.f, (m_collider->GetRadius() + m_collider->GetHalfHeight() - 5.f), 0.f);

	if (query->Raycast(hit, ray, 1 << static_cast<uint8_t>(PhysicsLayers::MAP), PhysicsQueryType::All, m_body))
	{
		return true;
	}

	return false;
}

Collider* CustomController::GetColliderBelow()
{
	//Raycast를 통해 아래에 있는 콜라이더의 정보를 불러온다
	//실패 시 nullptr 반환

	auto device = PhysDevice::GetInstance();
	auto query = device->GetQuery();

	RaycastHit hit;
	PhysicsRay ray;
	ray.direction = PxVec3(0.f, -1.f, 0.f);
	ray.distance = 0.21f;			//45degree-raycast distance : 0.2
	ray.point = m_body->GetPosition()
		- PxVec3(0.f, (m_collider->GetRadius() + m_collider->GetHalfHeight()), 0.f);

	if (query->Raycast(hit, ray, 1 << static_cast<uint8_t>(PhysicsLayers::MAP), PhysicsQueryType::Collider, m_body))
	{
		return hit.collider;
	}

	else
	{
		return nullptr;
	}
}

void CustomController::DirectionInput_Player()
{
	bool serverConnected = true;							//ONOFF

	if (serverConnected)
	{
		m_moveDirection = PxVec3(0.f, 0.f, 0.f);

		PxVec3 up = PxVec3(0.f, 1.f, 0.f);
		m_cameraLook.y = 0.f;		//카메라의 y성분 제거
		m_cameraLook.normalize();
		PxVec3 right = up.cross(m_cameraLook);

		if (m_keyboardInput[A].press)
			m_moveDirection -= right;
		else if (m_keyboardInput[D].press)
			m_moveDirection += right;

		if (m_keyboardInput[W].press)
			m_moveDirection += m_cameraLook;
		else if (m_keyboardInput[S].press)
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
			m_moveDirection.z = 1.f;
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
			m_moveDirection.z = -1.f;

		m_moveDirection.normalize();
	}
}

//나중에 수정
void CustomController::DirectionInput_Monster(physx::PxVec3 targetDir)
{
	m_moveDirection = targetDir;

	m_moveDirection.normalize();

}

void CustomController::Movement_Player()
{
	if (m_body->isKinematic())
		return;

	//check onGround

	GetSlidingVector(CollisionInfoType::Stay);
	PxVec3 surfaceNormal{ 0.f };

	CheckOnGround(CollisionInfoType::Enter, surfaceNormal);
	CheckOnGround(CollisionInfoType::Stay, surfaceNormal);
	
	//if ((GetAsyncKeyState(VK_SPACE) & 0x8000) && m_onGround)		//ONOFF
	if (m_keyboardInput[SPACE].down && m_onGround)
	{
		PxVec3 up{ 0.f, 1.f, 0.f };
		m_body->GetPosition() += up * 0.05f;

		PxVec3 velocity = m_body->GetVelocity();
		velocity.y = m_jumpSpeed;
		m_body->SetVelocity(velocity);

		m_onGround = false;
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
		if (!m_BounceFromAttack)
		{	//에어본 공격 당하면 해당 코드 진입 막음
			m_body->SetVelocity(m_slidingVector);	// * movespeed?
		}
		else
		{
		}
	}
	else
	{
		m_moveDirection *= m_moveSpeed;
		m_moveDirection.y = m_body->GetVelocity().y;
		if (!m_BounceFromAttack)
		{	//에어본 공격 당하면 해당 코드 진입 막음
			m_body->SetVelocity(m_moveDirection);
		}
		else
		{
		}
	}

	m_slidingVector = PxVec3(0.f);
	m_moveDirection = PxVec3(0.f);
}

void CustomController::Movement_Monster()
{
	if (m_body->isKinematic())
		return;

	//check onGround
	GetSlidingVector(CollisionInfoType::Stay);
	PxVec3 surfaceNormal{ 0.f };
	CheckOnGround(CollisionInfoType::Stay, surfaceNormal);

	//if ((GetAsyncKeyState(VK_SPACE) & 0x8000) && m_onGround)
	if (m_keyboardInput[SPACE].press && m_onGround)
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
		m_keyboardInput[SPACE].press = false;
	}

	//friction adjustment
	if (!m_onGround)
		m_collider->SetFriction(0.f);
	else
		m_collider->SetFriction(1.f);

	float newMoveSpeed = m_moveSpeed * m_moveSpeedScale;

	// Apply the adjusted vector to the character's velocity
	if (m_slidingVector.magnitude() > 0)
	{
		m_slidingVector *= newMoveSpeed;
		m_slidingVector.y = m_body->GetVelocity().y;
		m_body->SetVelocity(m_slidingVector);	// * movespeed?
	}
	else
	{
		m_moveDirection *= newMoveSpeed;
		m_moveDirection.y = m_body->GetVelocity().y;
		m_body->SetVelocity(m_moveDirection);
	}

	m_slidingVector = PxVec3(0.f);
	m_moveDirection = PxVec3(0.f);
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
	for (int32_t i = 0; i < KEY_ORDER::MAX; ++i)
	{
		int32_t temp{ 3 };

		temp <<= (i * 2);
		temp &= key;
		temp >>= (i * 2);

		switch (magic_enum::enum_value<server::KEY_STATE>(temp))
		{
			case server::KEY_STATE::PRESS:
			{
				m_keyboardInput[i].Press();
			}
			break;
			case server::KEY_STATE::DOWN:
			{
				m_keyboardInput[i].Down();
			}
			break;
			case server::KEY_STATE::UP:
			{
				m_keyboardInput[i].Up();
			}
			break;
			case server::KEY_STATE::NONE:
			{
				m_keyboardInput[i].None();
			}
			break;
			default:
			break;
		}
	}
}

void CustomController::SetMoveSpeed(float value)
{
	m_moveSpeed = value;
}

void CustomController::SetMoveSpeedScale(float value)
{
	m_moveSpeedScale = value;
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

float CustomController::GetDistanceFromGround()
{
	return m_distanceFromGround;
}

bool CustomController::Falling()
{
	PxVec3 curVel = m_body->GetVelocity();
	if (curVel.y < 0.f)
		return true;

	return false;
}

KeyInput& CustomController::GetKeyStatus(KEY_ORDER key)
{
	return m_keyboardInput[key];
}

KeyInput& CustomController::GetKeyStatus(int32_t key)
{
	return m_keyboardInput[key];
}

std::vector<KeyInput>& CustomController::GetKeyInput()
{
	return m_keyboardInput;
}

void CustomController::Keyboard_Direction_Clear()
{
	//FSM에서 directionInput에 제한을 걸기 위한 함수

	m_keyboardInput[KEY_ORDER::W].None();
	m_keyboardInput[KEY_ORDER::A].None();
	m_keyboardInput[KEY_ORDER::S].None();
	m_keyboardInput[KEY_ORDER::D].None();
}

void CustomController::Keyboard_SpaceBar_Clear()
{
	m_keyboardInput[KEY_ORDER::SPACE].None();
}

void CustomController::Keyboard_ATK_Clear()
{
	m_keyboardInput[KEY_ORDER::KEY_1].None();
	m_keyboardInput[KEY_ORDER::KEY_2].None();
	m_keyboardInput[KEY_ORDER::KEY_3].None();
	m_keyboardInput[KEY_ORDER::KEY_4].None();
}

void CustomController::BounceFromAttack()
{
	m_BounceFromAttack = true;
	auto dynamic = m_body->GetBody();
	auto curT = dynamic->getGlobalPose();
	curT.p.y += 10.f;
	dynamic->setGlobalPose(curT, true);
}

RigidBody* CustomController::GetBody()
{
	return m_body;
}

CapsuleCollider* CustomController::GetCollider()
{
	return m_collider;
}

bool CustomController::IsOnGroundByDistance()
{
	if (IsEqual(m_distanceFromGround, -1.f) == true)
		return false;

	return m_distanceFromGround < 30.f;
}

bool CustomController::IsOnGround()
{
	return m_onGround;
}

physx::PxVec3 CustomController::GetCameraLook()
{
	return m_cameraLook;
}
