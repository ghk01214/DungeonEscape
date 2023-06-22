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
	m_aniIndex{ 0 },
	m_aniFrame{ 0.f },
	m_aniSpeed{ 1.f },
	m_hp{ 20 }
{
	m_id = MonsterID;
}

Monster::~Monster()
{
}

void Monster::Init()
{
	m_controller = AddComponent<CustomController>(L"CustomController", false);

	auto body = m_controller->GetBody();
	body->SetMass(body->GetMass() * 0.7f);

	SetObjectType(server::OBJECT_TYPE::BOSS);

	std::cout << "BOSS HP : " << m_hp << "\n";
}

void Monster::Update(double timeDelta)
{
	// 몬스터 사망 시 삭제 로직 추가 필요
	m_controller->Move(m_id);

	if (m_startSendTransform == true)
	{
		game::Message msg{ m_id, ProtocolID::WR_TRANSFORM_ACK };
		game::MessageHandler::GetInstance()->PushTransformMessage(msg);
	}

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

void Monster::SetFBXType(server::FBX_TYPE fbxType)
{
	// 몬스터 별 IDLE 상태로 설정
	switch (fbxType)
	{
		case server::FBX_TYPE::WEEPER:
		{
			m_aniIndex = 11;
		}
		break;
		case server::FBX_TYPE::DRAGON:
		{
			m_aniIndex = 6;
		}
		break;
		default:
		break;
	}

	GameObject::SetFBXType(fbxType);
}

void Monster::SetAniInfo(int32_t aniIndex, float aniFrame, float aniSpeed)
{
	m_aniIndex = aniIndex;
	m_aniFrame = aniFrame;
	m_aniSpeed = aniSpeed;
}

bool Monster::IsOnGround()
{
	return m_controller->IsOnGround();
}

bool Monster::IsDead()
{
	return m_hp <= 0;
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
			continue;

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

		std::cout << "몬스터 패턴 발동, 플레이어 피격 적용\n";

		game::Message hitMsg{ -1, ProtocolID::WR_HIT_ACK };
		hitMsg.objID = result->GetID();
		hitMsg.objType = server::OBJECT_TYPE::PLAYER;
		hitMsg.hitID = m_id;

		game::MessageHandler::GetInstance()->PushSendMessage(hitMsg);

		//if (result->IsDead() == true)
		//{
		//	std::cout << "PLAYER [" << result->GetID() << "] DEAD!\n";

		//	game::Message deadMsg{ -1, ProtocolID::WR_DIE_ACK };
		//	deadMsg.playerID = result->GetID();
		//	deadMsg.objType = server::OBJECT_TYPE::PLAYER;

		//	game::MessageHandler::GetInstance()->PushSendMessage(deadMsg);

		//	// 플레이어 클래스 내에서 바꾸면 주석처리 or 삭제
		//	result->SetTransformSendFlag(false);
		//}

		//playerbody->SetVelocity(physx::PxVec3(direction.x * 100, 100, direction.z * 100));
	}
}

void Monster::GetDamaged(int32_t damage)
{
	m_hp -= damage;
}
