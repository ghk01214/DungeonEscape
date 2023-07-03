#include "pch.h"
#include "Monster.h"
#include "Scorpion.h"
#include "CustomController.h"
#include "MessageHandler.h"
#include "TimeManager.h"
#include "RigidBody.h"
#include "Transform.h"
#include "Collider.h"
#include "CollisionPairInfo.h"
#include "Player.h"
#include "RigidBody.h"

Scorpion::Scorpion(int32_t MonsterID, const Vec3& position, const Quat& rotation, const Vec3& scale) :
	Monster{ MonsterID, position, rotation, scale },
	m_prevState{ IDLE1 },
	m_currState{ m_prevState }
{
	m_hp = 20;

	m_name = L"Scorpion";
	m_objType = server::OBJECT_TYPE::BOSS;
	m_fbxType = server::FBX_TYPE::BLACK_SCORPION;
}

Scorpion::~Scorpion()
{
}

void Scorpion::Init()
{
	Monster::Init();

	//패턴 크기 다 넣기
	//AddSkillSize();
}

void Scorpion::Update(double timeDelta)
{
	if (IsDead() == true)
	{
		if (m_currState != DEAD)
			m_currState = DEATH;

		Monster::Update(timeDelta);

		return;
	}

	Monster::Update(timeDelta);
}

void Scorpion::LateUpdate(double timeDelta)
{
	CheckState();
	UpdateFrame();

	//Monster::LateUpdate(timeDelta);
}

void Scorpion::Release()
{
	Monster::Release();
}

void Scorpion::CheckState()
{
	if (m_prevState == m_currState)
		return;

	switch (m_currState)
	{
		case COMBAT_DEATH:
		{
			std::cout << "BOSS DEFEATED!\n";
		}
		break;
		case STUN:
		{

		}
		break;
		case SUBMERGE_ATTACK_LEFT:
		{

		}
		break;
		case SUBMERGE_ATTACK_MIDDLE:
		{

		}
		break;
		case SUBMERGE_ATTACK_RIGHT:
		{

		}
		break;
		case ATTACK1:
		{

		}
		break;
		case ATTACK2:
		{

		}
		break;
		case ATTACK3:
		{

		}
		break;
		case ATTACK4:
		{

		}
		break;
		case ATTACK_SPECIAL:
		{

		}
		break;
		case DODGE_LEFT:
		{

		}
		break;
		case DODGE_RIGHT:
		{

		}
		break;
		case DAMAGE:
		{

		}
		break;
		case READY:
		{

		}
		break;
		case DEATH:
		{
			std::cout << "BOSS DEFEATED!\n";
		}
		break;
		case DEAD:
		{
			SetRemoveReserved();

			game::Message msg{ -1, ProtocolID::WR_REMOVE_ACK };
			msg.objID = m_id;
			msg.objType = m_objType;

			game::MessageHandler::GetInstance()->PushSendMessage(msg);
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
		case IDLE_DAMAGE:
		{

		}
		break;
		case JUMP_END:
		{

		}
		break;
		case JUMP_LOOP:
		{

		}
		break;
		case JUMP_START:
		{

		}
		break;
		case RUN:
		{

		}
		break;
		case RUN_BACKWARD:
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
		case SUBMERGE_END:
		{

		}
		break;
		case SUBMERGE_LOOP:
		{

		}
		break;
		case SUBMERGE_START:
		{

		}
		break;
		case SWIM:
		{

		}
		break;
		case SWIM_BACKWARD:
		{

		}
		break;
		case SWIM_LEFT:
		{

		}
		break;
		case SWIM_RIGHT:
		{

		}
		break;
		case SWIM_DEATH:
		{
			std::cout << "BOSS DEFEATED!\n";
		}
		break;
		case SWIM_IDLE:
		{

		}
		break;
		case WALK:
		{

		}
		break;
		case WALK_BACKWARD:
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
	}

	m_prevState = m_currState;

	game::Message msg{ -1, ProtocolID::WR_CHANGE_STATE_ACK };
	msg.state = m_currState;
	msg.objID = m_id;
	msg.objType = m_objType;

	game::MessageHandler::GetInstance()->PushSendMessage(msg);
}

void Scorpion::UpdateFrameRepeat()
{
	switch (m_currState)
	{
		case COMBAT_DEATH: case SUBMERGE_ATTACK_LEFT: case SUBMERGE_ATTACK_MIDDLE: case SUBMERGE_ATTACK_RIGHT:
		case ATTACK1: case ATTACK2: case ATTACK3: case ATTACK4: case ATTACK_SPECIAL:
		case DODGE_LEFT: case DODGE_RIGHT: case DAMAGE: case DEATH: case DEAD: case SWIM_DEATH:
		case IDLE_DAMAGE: case JUMP_START: case JUMP_END: case SUBMERGE_START: case SUBMERGE_END:
		return;
		default:
		break;
	}
}

void Scorpion::UpdateFrame()
{
	switch (m_currState)
	{
		case STUN: case READY: case IDLE1: case IDLE2: case IDLE3: case SUBMERGE_LOOP:
		case JUMP_LOOP: case RUN: case RUN_BACKWARD: case RUN_LEFT: case RUN_RIGHT:
		case SWIM: case SWIM_BACKWARD: case SWIM_LEFT: case SWIM_RIGHT: case SWIM_IDLE:
		case WALK: case WALK_BACKWARD: case WALK_LEFT: case WALK_RIGHT:
		return;
		default:
		break;
	}

	if (m_aniEnd == false)
		return;

	switch (m_currState)
	{
		case ATTACK1: case ATTACK2: case ATTACK3: case ATTACK4: case ATTACK_SPECIAL:
		case DODGE_LEFT: case DODGE_RIGHT: case DAMAGE: case IDLE_DAMAGE:
		{
			m_currState = IDLE1;
		}
		break;
		case SUBMERGE_ATTACK_LEFT: case SUBMERGE_ATTACK_MIDDLE: case SUBMERGE_ATTACK_RIGHT:
		{
			m_currState = SUBMERGE_LOOP;
		}
		break;
		case COMBAT_DEATH: case DEATH: case SWIM_DEATH:
		{
			m_currState = DEAD;
			m_startSendTransform = false;
		}
		break;
		case JUMP_START: case SUBMERGE_START:
		{
			// END로 갈지 LOOP로 갈지는 추후 변경
			// 현재는 END로 가도록 설정
			m_currState = magic_enum::enum_value<SCORPION_STATE>(m_currState - 2);
		}
		break;
		case JUMP_END: case SUBMERGE_END:
		{
			m_currState = IDLE1;
		}
		break;
		default:
		break;
	}

	m_aniEnd = false;
}

Scorpion::SCORPION_STATE Scorpion::GetState() const
{
	return m_currState;
}

void Scorpion::SetState(SCORPION_STATE state)
{
	m_currState = state;
}
