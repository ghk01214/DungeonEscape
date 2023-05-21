#include "pch.h"
#include "Monster.h"
#include "CustomController.h"
#include "MessageHandler.h"
#include "TimeManager.h"
#include "RigidBody.h"
#include "Transform.h"
#include "Collider.h"
#include "CollisionPairInfo.h"
#include "Player.h"
#include "RigidBody.h"

Monster::Monster(int32_t MonsterID, const Vec3& position, const Quat& rotation, const Vec3& scale) :
	GameObject{ position, rotation, scale },
	m_MonsterID{ MonsterID },
	m_aniIndex{ 0 },
	m_aniFrame{ 0.f }
{
}

Monster::~Monster()
{
}

void Monster::Init()
{
	m_controller = AddComponent<CustomController>(L"CustomController", false);

	auto body = m_controller->GetBody();
	body->SetMass(body->GetMass() * 0.7f);
}

void Monster::Update(double timeDelta)
{
	m_controller->Move();

	if (m_startSendTransform == true)
	{
		game::Message msg{ m_MonsterID, ProtocolID::WR_TRANSFORM_ACK };
		game::MessageHandler::GetInstance()->PushTransformMessage(msg);
	}

	// 스테이트 변경
	// game::Message msg{ m_playerID, ProtocolID::WR_ANI_ACK };
	// msg.aniIndex = 애니 인덱스;
	// msg.aniFrame = 0.f;
	// game::MessageHandler::GetInstance()->PushTransformMessage(msg);

	GameObject::Update(timeDelta);
}

void Monster::LateUpdate(double timeDelta)
{
	m_controller->ClearControllerCollisionInfo();
	m_transform->ConvertPX(m_controller->GetBody()->GetGlobalPose());
}

void Monster::Release()
{
	m_controller = nullptr;
	GameObject::Release();
}

void Monster::SetAniInfo(int32_t aniIndex, float aniFrame)
{
	m_aniIndex = aniIndex;
	m_aniFrame = aniFrame;
}

void Monster::SetFBXType(server::FBX_TYPE fbxType)
{
	m_fbxType = fbxType;
}

void Monster::SetControllerMoveSpeed(float value)
{
	m_controller->SetMoveSpeed(value);
}

float Monster::GetControllerMoveSpeed()
{
	return m_controller->GetMoveSpeed();
}

void Monster::SetControllerJumpSpeed(float value)
{
	m_controller->SetJumpSpeed(value);
}

float Monster::GetControllerJumpSpeed()
{
	return m_controller->GetJumpSpeed();
}

void Monster::SetControllerCameraLook(Vec3& value)
{
	m_controller->CameraLookReceive(value);
}

void Monster::MonsterPattern_GroundHit()
{
	Collider* surfaceCollider = m_controller->GetColliderBelow();
	if (surfaceCollider == nullptr)
		return;

	physx::PxVec3 monsterPos = m_controller->GetBody()->GetPosition();

	auto collisionEnterLists = surfaceCollider->GetCollisionInfo(CollisionInfoType::Enter);
	auto collisionStayLists = surfaceCollider->GetCollisionInfo(CollisionInfoType::Stay);

	std::set<Collider*> enTouch;		//몬스터와 동일한 메쉬에 서있는 콜라이더들

	for (auto& info : collisionStayLists)
	{
		auto collider = info.get()->GetFromCollider();
		auto obj = collider->GetOwnerObject();
		auto result = dynamic_cast<Player*>(obj);
		if (result == nullptr)
		{
			continue;
		}
		else
		{
			auto playerController = result->GetComponent<CustomController>(L"CustomController");
			auto playerbody = playerController->GetBody();
			physx::PxVec3 playerPos = playerbody->GetPosition();
			physx::PxVec3 direction = playerPos - monsterPos;
			float distance = direction.magnitude();

			direction.normalize();

			float verticalStrength = 1.f;
			float horizontalStrength = 1.f;
			if (distance < 300)
			{
				horizontalStrength = 300.f;
				verticalStrength = 1000.f;
			}
			else
			{
				horizontalStrength = 100.f;
				verticalStrength = 600.f;
			}
			playerController->BounceFromAttack();
			playerbody->AddForce(ForceMode::Impulse, physx::PxVec3(direction.x * horizontalStrength, verticalStrength, direction.z * horizontalStrength));
			//playerbody->SetVelocity(physx::PxVec3(direction.x * 100, 100, direction.z * 100));
		}
	}
}
