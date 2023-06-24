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
	m_prevState{ IDLE1 },
	m_currState{ m_prevState },
	m_aniPlayTime{ 0.f },
	m_aniEnd{ false },
	m_hp{ 20 },
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

	for (int32_t i = magic_enum::enum_integer(server::TRIGGER_TYPE::NONE) + 1; i < magic_enum::enum_integer(server::TRIGGER_TYPE::MAX); ++i)
	{
		m_triggerDictionary[magic_enum::enum_value<server::TRIGGER_TYPE>(i)] = false;
	}
}

void Player::Update(double timeDelta)
{
	if (m_hp <= 0)
	{
		m_currState = DIE0;
		GameObject::Update(timeDelta);

		return;
	}

	ChangeStateByKeyInput();

	m_controller->Move(m_id);

	IsOnGound();
	PlayerPattern_SingleStrike();
	PlayerPattern_ShootBall(server::OBJECT_TYPE::FIREBALL, 100.f);
	TriggerZoneStatusUpdate();

	GameObject::Update(timeDelta);

	SendTransform();
}

void Player::LateUpdate(double timeDelta)
{
	CheckState();
	UpdateFrame();

	m_controller->ClearControllerCollisionInfo();
	m_transform->ConvertPX(m_controller->GetBody()->GetGlobalPose());
}

void Player::Release()
{
	m_controller = nullptr;
	GameObject::Release();
}

void Player::IsOnGound()
{
	auto distance{ m_controller->GetDistanceFromGround() };

	// 수정
	if (IsEqual(distance, -1.f) == true or distance > 30.f)
		m_currState = JUMPING;
}

bool Player::IsDead()
{
	return m_hp <= 0;
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

Player::PLAYER_STATE Player::GetState() const
{
	return m_currState;
}

float Player::GetAniPlayTime() const
{
	return m_aniPlayTime;
}

void Player::SetState(PLAYER_STATE state)
{
	m_currState = state;
}

void Player::SetState(int32_t index)
{
	m_currState = magic_enum::enum_value<PLAYER_STATE>(index);
}

void Player::SetAniPlayTime(float time)
{
	m_aniPlayTime = time;
}

void Player::ChangeAniEndFlag(bool flag)
{
	m_aniEnd = flag;
}

void Player::GetDamaged(int32_t damage)
{
	m_hp -= damage;
}

void Player::ChangeStateByKeyInput()
{
	switch (m_currState)
	{
		case ATK0: case ATK1: case ATK2: case ATK3: case ATK4:
		case JUMP_START:case DAMAGE: case DEAD:
		case DIE0: case DIE1: case DIE2:
		return;
		default:
		break;
	}

	auto key{ m_controller->GetKeyInput() };

	if (key[W].down == true or key[W].press == true
		or key[S].down == true or key[S].press == true)
	{
		m_currState = MOVE;
	}
	else if (key[A].down == true or key[A].press == true)
	{
		m_currState = MOVE;
	}
	else if (key[D].down == true or key[D].press == true)
	{
		m_currState = MOVE;
	}
	else if (key[W].up == true or key[S].up == true
		or key[A].up == true or key[D].up == true)
	{
		m_currState = IDLE1;
	}

	if (key[SPACE].down == true and m_controller->IsOnGround() == true)
	{
		m_currState = JUMP_START;
	}
	else if (key[KEY_1].down == true)
	{
		m_currState = ATK0;
	}
	else if (key[KEY_2].down == true)
	{
		m_currState = ATK1;
	}
	else if (key[KEY_3].down == true)
	{
		m_currState = ATK2;
	}
	else if (key[KEY_4].down == true)
	{
		m_currState = ATK3;
	}
}

void Player::CheckState()
{
	if (m_prevState == m_currState)
		return;

	switch (m_currState)
	{
		case AERT:
		{
		}
		break;
		case ATK0:
		{

		}
		break;
		case ATK1:
		{

		}
		break;
		case ATK2:
		{

		}
		break;
		case ATK3:
		{

		}
		break;
		case ATK4:
		{

		}
		break;
		case BLOCK:
		{

		}
		break;
		case DAMAGE:
		{

		}
		break;
		case DASH:
		{

		}
		break;
		case DIE0:
		{

		}
		break;
		case DIE1:
		{

		}
		break;
		case DIE2:
		{

		}
		break;
		case IDLE1:
		{

		}
		break;
		case IDLE2:
		{

		}
		break;
		case IDLE3:
		{

		}
		break;
		case JUMP_START:
		{

		}
		break;
		case MOVE:
		{

		}
		break;
		case MOVE_LEFT:
		{

		}
		break;
		case MOVE_RIGHT:
		{

		}
		break;
		case REST:
		{

		}
		break;
		case RUN:
		{

		}
		break;
		case RUN_LEFT:
		{

		}
		break;
		case RUN_RIGHT:
		{

		}
		break;
		case SHOOT:
		{

		}
		break;
		case SLEEP:
		{

		}
		break;
		case SWOON:
		{

		}
		break;
		case TIRED:
		{

		}
		break;
		case VICTORY1:
		{

		}
		break;
		case VICTORY2:
		{

		}
		break;
		case WALK:
		{

		}
		break;
		case WALK_LEFT:
		{

		}
		break;
		case WALK_RIGHT:
		{

		}
		break;
		default:
		break;
	}

	m_prevState = m_currState;

	game::Message msg{ m_id, ProtocolID::WR_CHANGE_STATE_ACK };
	msg.state = m_currState;
	msg.objType = m_objType;

	game::MessageHandler::GetInstance()->PushSendMessage(msg);
}

void Player::UpdateFrame()
{
	switch (m_currState)
	{
		case IDLE1: case IDLE2: case IDLE3:
		case MOVE: case MOVE_LEFT: case MOVE_RIGHT:
		case RUN: case RUN_LEFT: case RUN_RIGHT:
		case WALK: case WALK_LEFT: case WALK_RIGHT:
		case VICTORY1: case VICTORY2: case DEAD:
		return;
		default:
		break;
	}

	if (m_aniEnd == true)
	{
		switch (m_currState)
		{
			case ATK0: case ATK1: case ATK2: case ATK3: case ATK4:
			{
				m_currState = IDLE1;
			}
			break;
			case DAMAGE:
			{
				m_currState = IDLE1;
			}
			break;
			case DIE0: case DIE1: case DIE2:
			{
				m_currState = DEAD;
				m_startSendTransform = false;
			}
			break;
			case JUMP_END:
			{
				m_currState = IDLE1;
			}
			break;
			case SHOOT:
			{
				m_currState = IDLE1;
			}
			break;
			case SLEEP:
			{
				m_currState = IDLE1;
			}
			break;
			case SWOON:
			{
				m_currState = IDLE1;
			}
			break;
			case TIRED:
			{
				m_currState = IDLE1;
			}
			break;
			default:
			break;
		}

		m_aniEnd = false;
		return;
	}

	if (m_controller->IsOnGround() == true)
	{
		if (m_currState == JUMP_START or m_currState == JUMPING)
			m_currState = JUMP_END;
	}
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

void Player::PlayerPattern_ShootBall(server::OBJECT_TYPE type, float power)
{
	if (m_fbxType != server::FBX_TYPE::MISTIC)
		return;

	int32_t keyIndex{ IsAttackKeyDown() };

	if (keyIndex == -1)
		return;

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

	int32_t objID{ game::MessageHandler::GetInstance()->NewObjectID() };

	auto ballObject = objmgr->AddGameObjectToLayer<SkillObject>(L"Layer_SkillObject", ballPos, Quat(0, 0, 0, 1), Vec3(skillBallHalfExtent, skillBallHalfExtent, skillBallHalfExtent));
	auto ballBody = ballObject->GetComponent<RigidBody>(L"RigidBody");
	ballBody->SetMass(1.f);
	ballBody->AddCollider<SphereCollider>(ballObject->GetTransform()->GetScale());
	ballObject->SetID(objID);
	ballObject->SetName(L"Sphere");
	ballObject->SetFBXType(server::FBX_TYPE::SPHERE);
	ballObject->SetObjectType(type);

	ballBody->AddForce(ForceMode::Impulse, playerCameraLook * power);

	game::Message sendMsg{ -1, ProtocolID::WR_ADD_OBJ_ACK };
	sendMsg.objType = server::OBJECT_TYPE::FIREBALL;
	sendMsg.objID = objID;

	game::MessageHandler::GetInstance()->PushSendMessage(sendMsg);

	// KeyUp 상태가 전달되기까지의 딜레이가 있어서 로직 종료 시 key 상태 변경
	m_controller->GetKeyStatus(keyIndex).None();
}

void Player::PlayerPattern_SingleStrike()
{
	if (m_fbxType != server::FBX_TYPE::NANA)
		return;

	int32_t keyIndex{ IsAttackKeyDown() };

	if (keyIndex == -1)
		return;

	enum { start, end };
	std::array<float, 2> attackTime;

	switch (keyIndex)
	{
		case KEY_1:
		{
			attackTime[start] = 0.2f;
			attackTime[end] = 0.43f;
		}
		break;
		case KEY_2:
		{
			attackTime[start] = 0.2f;
			attackTime[end] = 0.39f;
		}
		break;
		case KEY_3:
		{
			attackTime[start] = 0.08f;
			attackTime[end] = 0.19f;
		}
		break;
		case KEY_4:
		{
			attackTime[start] = 0.6f;
			attackTime[end] = 0.75f;
		}
		break;
		//case KEY_5:
		//{
		//	attackTime[start] = 0.08f;
		//	attackTime[end] = 0.27f;
		//}
		//break;
		default:
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

	m_controller->GetKeyStatus(keyIndex).None();
}

int32_t Player::IsAttackKeyDown()
{
	for (int32_t i = KEY_1; i <= KEY_4; ++i)
	{
		if (m_controller->GetKeyStatus(i).down == true)
			return i;
	}

	return -1;
}

void Player::SendTransform()
{
	if (m_startSendTransform == false)
		return;

	game::Message msg{ m_id, ProtocolID::WR_TRANSFORM_ACK };
	game::MessageHandler::GetInstance()->PushTransformMessage(msg);
}
