#include "pch.h"
#include "Monster.h"
#include "MonsterAI.h"
#include "ObjectManager.h"
#include "CustomController.h"
#include "MessageHandler.h"
#include "TimeManager.h"
#include "RigidBody.h"
#include "Transform.h"
#include "Collider.h"
#include "CollisionPairInfo.h"
#include "Player.h"
#include "RigidBody.h"
#include "Layer.h"

Monster::Monster(int32_t MonsterID, const Vec3& position, const Quat& rotation, const Vec3& scale) :
	GameObject{ position, rotation, scale },
	m_controller{ nullptr },
	m_aniPlayTime{ 0.f },
	m_aniEnd{ false }
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
}

void Monster::Update(double timeDelta)
{
	GameObject::Update(timeDelta);

	SendTransform();
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

Vec3 Monster::GetControllerPosition()
{
	return FROM_PX3(m_controller->GetBody()->GetPosition());
}

void Monster::SetControllerPosition(Vec3 pos)
{
	m_controller->GetBody()->SetPosition(pos, true);
}

int32_t Monster::GetHP() const
{
	return m_hp;
}

float Monster::GetAniPlayTime() const
{
	return m_aniPlayTime;
}

void Monster::SetFBXType(server::FBX_TYPE fbxType)
{
	// 몬스터 별 IDLE 상태로 설정
	GameObject::SetFBXType(fbxType);
}

void Monster::SetAniPlayTime(float time)
{
	m_aniPlayTime = time;
}

void Monster::SetAniEndFlag(bool flag)
{
	m_aniEnd = flag;
}

bool Monster::IsOnGround()
{
	return m_controller->IsOnGroundByDistance();
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

void Monster::GetDamaged(int32_t damage)
{
	m_hp -= damage;
}

void Monster::SendTransform()
{
	if (m_startSendTransform == true)
	{
		game::Message msg{ m_id, ProtocolID::WR_TRANSFORM_ACK };
		game::MessageHandler::GetInstance()->PushTransformMessage(msg);
	}
}
