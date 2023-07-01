#include "pch.h"
#include "Monster.h"
#include "Golem.h"
#include "CustomController.h"
#include "MessageHandler.h"
#include "TimeManager.h"
#include "RigidBody.h"
#include "Transform.h"
#include "Collider.h"
#include "CollisionPairInfo.h"
#include "Player.h"
#include "RigidBody.h"

Golem::Golem(int32_t MonsterID, const Vec3& position, const Quat& rotation, const Vec3& scale) :
	Monster{ MonsterID, position, rotation, scale },
	m_prevState{ IDLE1 },
	m_currState{ m_prevState }
{
	m_hp = 20;

	m_name = L"Golem";
	m_objType = server::OBJECT_TYPE::BOSS;
	m_fbxType = server::FBX_TYPE::RED_GOLEM;
}

Golem::~Golem()
{
}

void Golem::Init()
{
	Monster::Init();

	//패턴 크기 다 넣기
	//AddSkillSize();
}

void Golem::Update(double timeDelta)
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

void Golem::LateUpdate(double timeDelta)
{
	CheckState();
	UpdateFrame();

	Monster::LateUpdate(timeDelta);
}

void Golem::Release()
{
	Monster::Release();
}

void Golem::CheckState()
{
	if (m_prevState == m_currState)
		return;

	switch (m_currState)
	{
		case ROAR:
		{

		}
		break;
		case SPELL_START:
		{

		}
		break;
		case SPELL_LOOP:
		{

		}
		break;
		case SPELL_END:
		{

		}
		break;
		case STUN:
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
		case DAMAGE:
		{

		}
		break;
		case READY:
		{

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
		case DEATH:
		{
			std::cout << "BOSS DEFEATED!\n";
		}
		break;
		case LAUGH:
		{

		}
		break;
		case TALK:
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
		case JUMP_START:
		{

		}
		break;
		case JUMP_LOOP:
		{

		}
		break;
		case JUMP_END:
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
		default:
		break;
	}

	m_prevState = m_currState;

	game::Message msg{ -1, ProtocolID::WR_CHANGE_STATE_ACK };
	msg.state = m_currState;
	msg.objID = m_id;
	msg.objType = m_objType;

	game::MessageHandler::GetInstance()->PushSendMessage(msg);
}

void Golem::UpdateFrameRepeat()
{
	switch (m_currState)
	{
		case ROAR: case SPELL_START: case SPELL_END: case STUN:
		case ATTACK1: case ATTACK2: case ATTACK3: case ATTACK4:
		case DAMAGE: case READY: case DEATH: case DEAD:
		case JUMP_START: case JUMP_END:
		return;
		default:
		break;
	}
}

void Golem::UpdateFrame()
{
	switch (m_currState)
	{
		case SPELL_LOOP: case LAUGH: case TALK:
		case IDLE1: case IDLE2: case JUMP_LOOP: case SWIM_IDLE:
		case RUN: case RUN_BACKWARD: case RUN_LEFT: case RUN_RIGHT:
		case SWIM: case SWIM_BACKWARD: case SWIM_LEFT: case SWIM_RIGHT:
		case WALK: case WALK_BACKWARD: case WALK_LEFT: case WALK_RIGHT:
		return;
		default:
		break;
	}

	if (m_aniEnd == false)
		return;

	switch (m_currState)
	{
		case ROAR: case STUN: case DAMAGE:
		{
			m_currState = IDLE1;
		}
		break;
		case SPELL_START: case JUMP_START:
		{
			// END로 갈지 LOOP로 갈지는 추후 변경
			// 현재는 END로 가도록 설정
			m_currState = magic_enum::enum_value<GOLEM_STATE>(m_currState - 2);
		}
		break;
		case SPELL_END: case JUMP_END:
		{
			m_currState = IDLE1;
		}
		break;
		case ATTACK1: case ATTACK2: case ATTACK3: case ATTACK4:
		{
			m_currState = IDLE1;
		}
		break;
		case READY:
		{
			m_currState = ROAR;
		}
		break;
		case DEATH:
		{
			m_currState = DEAD;
			m_startSendTransform = false;
		}
		break;
		default:
		break;
	}

	m_aniEnd = false;
}

Golem::GOLEM_STATE Golem::GetState() const
{
	return m_currState;
}

void Golem::SetState(GOLEM_STATE state)
{
	m_currState = state;
}
