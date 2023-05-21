#include "pch.h"
#include "Player.h"
#include "CustomController.h"
#include "MessageHandler.h"
#include "ObjectManager.h"
#include "TimeManager.h"
#include "RigidBody.h"
#include "Transform.h"
#include "CapsuleCollider.h"
#include "SkillObject.h"
#include "SphereCollider.h"

Player::Player(int32_t playerID, const Vec3& position, const Quat& rotation, const Vec3& scale) :
	GameObject{ position, rotation, scale },
	m_playerID{ playerID },
	m_aniIndex{ 12 },
	m_aniFrame{ 0.f },
	m_aniSpeed{ 1.f }
{
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
}

void Player::Update(double timeDelta)
{
	m_controller->Move();

	if (m_startSendTransform == true)
	{
		game::Message msg{ m_playerID, ProtocolID::WR_TRANSFORM_ACK };
		game::MessageHandler::GetInstance()->PushTransformMessage(msg);

		if (m_controller->IsStartJump() == true)
		{
			msg.msgProtocol = ProtocolID::WR_JUMP_START_ACK;
			game::MessageHandler::GetInstance()->PushSendMessage(msg);
		}

		//auto p{ GetTransform()->GetPosition() };
		//std::cout << m_playerID << " : " << p.x << ", " << p.y << ", " << p.z << "\n";
	}

	// 스테이트 변경
	// game::Message msg{ m_playerID, ProtocolID::WR_ANI_ACK };
	// msg.aniIndex = 애니 인덱스;
	// msg.aniFrame = 0.f;
	// game::MessageHandler::GetInstance()->PushTransformMessage(msg);

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

void Player::SetAniInfo(int32_t aniIndex, float aniFrame, float aniSpeed)
{
	m_aniIndex = aniIndex;
	m_aniFrame = aniFrame;
	m_aniSpeed = aniSpeed;
}

void Player::SetControllerMoveSpeed(float value)
{
	m_controller->SetMoveSpeed(value);
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

void Player::PlayerPattern_ShootBall(float power)
{
	//투사체 위치 선정
	physx::PxVec3 playerPos = m_controller->GetBody()->GetGlobalPose().p;
	physx::PxVec3 playerCameraLook = m_controller->GetCameraLook().getNormalized();

	float playerRadius = m_controller->GetCollider()->GetRadius();
	float skillBallHalfExtent = 50.f;
	
	physx::PxVec3 skillBallPosition = playerPos + playerCameraLook * (playerRadius + skillBallHalfExtent + 10);
	Vec3 ballPos = FROM_PX(skillBallPosition);

	//투사체 생성
	auto objmgr = ObjectManager::GetInstance();
	auto layer = objmgr->GetLayer(L"Layer_SkillObject");


	auto ballObject = objmgr->AddGameObjectToLayer<SkillObject>(L"Layer_SkillObject", ballPos, Quat(0, 0, 0, 1), Vec3(skillBallHalfExtent, skillBallHalfExtent, skillBallHalfExtent));
	auto ballBody = ballObject->GetComponent<RigidBody>(L"RigidBody");
	ballBody->SetMass(1.f);
	ballBody->AddCollider<SphereCollider>(ballObject->GetTransform()->GetScale());

	ballBody->AddForce(ForceMode::Impulse, playerCameraLook * power);
}
