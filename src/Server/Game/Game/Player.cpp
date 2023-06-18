#include "pch.h"
#include "Player.h"
#include "CustomController.h"
#include "MessageHandler.h"
#include "ObjectManager.h"
#include "TimeManager.h"
#include "RigidBody.h"
#include "Transform.h"
#include "SkillObject.h"
#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "TriggerObject.h"

Player::Player(int32_t playerID, const Vec3& position, const Quat& rotation, const Vec3& scale) :
	GameObject{ position, rotation, scale },
	m_aniIndex{ 12 },
	m_aniFrame{ 0.f },
	m_aniSpeed{ 1.f },
	m_hp{ 20 },
	m_die{ false },
	m_firstSingleStrike{ true }
{
	m_id = playerID;
}

Player::~Player()
{
}

void Player::Init()
{
	m_controller = AddComponent<CustomController>(L"CustomController", true);

	auto body = m_controller->GetBody();
	body->SetMass(body->GetMass() * 0.7f);
	SetObjectType(server::OBJECT_TYPE::PLAYER);


	for (int i = static_cast<int>(server::TRIGGER_TYPE::NONE) + 1; i < static_cast<int>(server::TRIGGER_TYPE::MAX); ++i)
	{
		m_triggerDictionary[static_cast<server::TRIGGER_TYPE>(i)] = false;
	}
}

void Player::Update(double timeDelta)
{
	static uint64_t prev_num{ 0 };

	if (m_die == false)
	{
		m_controller->Move();

		if (m_startSendTransform == true)
		{
			game::Message msg{ m_id, ProtocolID::WR_TRANSFORM_ACK };
			game::MessageHandler::GetInstance()->PushTransformMessage(msg);

			if (m_damaged == false and m_controller->IsStartJump() == true)
			{
				msg.msgProtocol = ProtocolID::WR_JUMP_START_ACK;
				game::MessageHandler::GetInstance()->PushSendMessage(msg);
			}

			//auto p{ GetTransform()->GetPosition() };
			//std::cout << m_playerID << " : " << p.x << ", " << p.y << ", " << p.z << "\n";
		}

		if (m_damaged == true and m_controller->IsOnGround() == true)
			m_damaged = false;
	}

	TriggerZoneStatusUpdate();

	GameObject::Update(timeDelta);
}

void Player::LateUpdate(double timeDelta)
{
	m_controller->ClearControllerCollisionInfo();
	m_transform->ConvertPX(m_controller->GetBody()->GetGlobalPose());
}

void Player::Release()
{
	m_controller = nullptr;
	GameObject::Release();
}

bool Player::IsOnGound()
{
	return m_controller->IsOnGround();
}

bool Player::IsDead()
{
	return m_die;
}

void Player::TriggerZoneStatusChange(server::TRIGGER_TYPE triggerType, bool status)
{
	//trigger enter/exit에 대한 처리
	if (triggerType != server::TRIGGER_TYPE::NONE)
	{
		m_triggerDictionary[triggerType] = status;
	}


	//마그마블럭 진입/탈출
	if (triggerType == server::TRIGGER_TYPE::MAGMA)
	{
		if (status)
		{
			m_controller->SetMoveSpeed(20.f);		//default:30
			m_controller->SetJumpSpeed(50.f);		//default:70
		}
		else
		{
			m_controller->SetMoveSpeed(30.f);
			m_controller->SetJumpSpeed(70.f);
		}
	}
}

void Player::TriggerZoneStatusUpdate()
{
	//영역 내 지속적인 효과를 주는 트리거 상호작용만 포함
	//1회성 트리거(타격)의 경우 TriggerObject 클래스에서 플레이어의 status를 변경하는 방식으로 사용
	Trigger_Magma();
	Trigger_Wind();
}

void Player::Trigger_Magma()
{
	//특정 시간마다 데미지 부여(클라)
}

void Player::Trigger_Wind()
{
	if (!(m_triggerDictionary[server::TRIGGER_TYPE::WIND_FRONT] || m_triggerDictionary[server::TRIGGER_TYPE::WIND_BACK] ||
		m_triggerDictionary[server::TRIGGER_TYPE::WIND_LEFT] || m_triggerDictionary[server::TRIGGER_TYPE::WIND_RIGHT] ||
		m_triggerDictionary[server::TRIGGER_TYPE::WIND_UP] || m_triggerDictionary[server::TRIGGER_TYPE::WIND_DOWN]))
		return;

	if (m_triggerDictionary[server::TRIGGER_TYPE::WIND_UP])
	{
		m_controller->GetBody()->AddForce(ForceMode::Impulse, physx::PxVec3(0, 1.f, 0) * 1.f);
	}
}


void Player::SetAniInfo(int32_t aniIndex, float aniFrame, float aniSpeed)
{
	m_aniIndex = aniIndex;
	m_aniFrame = aniFrame;
	m_aniSpeed = aniSpeed;
}

void Player::GotHit(int32_t damage)
{
	m_hp -= damage;
	m_damaged = true;
}

void Player::KillPlayer()
{
	m_die = true;
}

void Player::SetControllerMoveSpeed(float value)
{
	m_controller->SetMoveSpeed(value);
}

void Player::SetControllerPosition(Vec3 pos)
{
	m_controller->GetBody()->SetPosition(pos, true);
}

Vec3 Player::GetControllerPosition()
{
	return FROM_PX3(m_controller->GetBody()->GetPosition());
}

Quat Player::GetControllerRotation()
{
	return FROM_PXQUAT(m_controller->GetBody()->GetRotation());
}

float Player::GetControllerMoveSpeed()
{
	return m_controller->GetMoveSpeed();
}

void Player::SetControllerJumpSpeed(float value)
{
	m_controller->SetJumpSpeed(value);
}

float Player::GetControllerJumpSpeed()
{
	return m_controller->GetJumpSpeed();
}

CustomController* Player::GetController()
{
	return m_controller;
}

void Player::SetControllerCameraLook(Vec3& value)
{
	m_controller->CameraLookReceive(value);
}

void Player::PlayerPattern_ShootBall(server::OBJECT_TYPE type, int32_t objID, float power)
{
	//투사체 위치 선정
	physx::PxVec3 playerPos = m_controller->GetBody()->GetGlobalPose().p;
	physx::PxVec3 playerCameraLook = m_controller->GetCameraLook().getNormalized();

	float playerRadius = m_controller->GetCollider()->GetRadius();
	float skillBallHalfExtent = 50.f;

	physx::PxVec3 skillBallPosition = playerPos + playerCameraLook * (playerRadius + skillBallHalfExtent + 10);
	Vec3 ballPos = FROM_PX3(skillBallPosition);

	//투사체 생성
	auto objmgr = ObjectManager::GetInstance();
	auto layer = objmgr->GetLayer(L"Layer_SkillObject");


	auto ballObject = objmgr->AddGameObjectToLayer<SkillObject>(L"Layer_SkillObject", ballPos, Quat(0, 0, 0, 1), Vec3(skillBallHalfExtent, skillBallHalfExtent, skillBallHalfExtent));
	auto ballBody = ballObject->GetComponent<RigidBody>(L"RigidBody");
	ballBody->SetMass(1.f);
	ballBody->AddCollider<SphereCollider>(ballObject->GetTransform()->GetScale());
	ballObject->SetID(objID);
	ballObject->SetName(L"Sphere");
	ballObject->SetFBXType(server::FBX_TYPE::SPHERE);
	ballObject->SetObjectType(type);

	ballBody->AddForce(ForceMode::Impulse, playerCameraLook * power);
}

void Player::PlayerPattern_SingleStrike(server::ATTACK_TYPE attack)
{
	enum { start, end };
	std::array<float, 2> attackTime;

	switch (attack)
	{
		case server::ATTACK_TYPE::ATK0:
		{
			attackTime[start] = 0.2f;
			attackTime[end] = 0.43f;
		}
		break;
		case server::ATTACK_TYPE::ATK1:
		{
			attackTime[start] = 0.2f;
			attackTime[end] = 0.39f;
		}
		break;
		case server::ATTACK_TYPE::ATK2:
		{
			attackTime[start] = 0.08f;
			attackTime[end] = 0.19f;
		}
		break;
		case server::ATTACK_TYPE::ATK3:
		{
			attackTime[start] = 0.6f;
			attackTime[end] = 0.75f;
		}
		break;
		case server::ATTACK_TYPE::ATK4:
		{
			attackTime[start] = 0.08f;
			attackTime[end] = 0.27f;
		}
		break;
	}

	auto objmgr = ObjectManager::GetInstance();
	physx::PxVec3 playerPos = TO_PX3(GetControllerPosition());				//플레이어 위치
	physx::PxVec3 cameraDir = m_controller->GetCameraLook();
	cameraDir.y = 0;  // y 컴포넌트를 0으로 설정
	cameraDir.normalize();  // 벡터를 정규화
	float angle = physx::PxAtan2(cameraDir.z, cameraDir.x);
	physx::PxQuat cameraRot(angle, physx::PxVec3(0, 1, 0));					//카메라 회전정보

	float radius = m_controller->GetCollider()->GetRadius();
	float extent = 100.f;
	physx::PxVec3 triggerPos = playerPos + cameraDir * (radius + extent);	//트리거 위치

	if (m_firstSingleStrike == true)
	{
		m_attackTrigger = objmgr->AddGameObjectToLayer<TriggerObject>(L"Trigger", FROM_PX3(triggerPos), FROM_PXQUAT(cameraRot), Vec3(extent, extent, extent));
		m_attackTrigger->SetTriggerType(server::TRIGGER_TYPE::SINGLE_STRIKE, attackTime[start], attackTime[end]);
		auto triggerBody = m_attackTrigger->GetComponent<RigidBody>(L"RigidBody");
		triggerBody->AddCollider<BoxCollider>(m_attackTrigger->GetTransform()->GetScale());
		triggerBody->GetCollider(0)->SetTrigger(true);
		m_firstSingleStrike = false;
	}
	else
	{
		m_attackTrigger->RestoreOneTimeEffect();
		auto triggerBody = m_attackTrigger->GetComponent<RigidBody>(L"RigidBody");
		triggerBody->SetPosition(FROM_PX3(triggerPos), true);
	}
}
