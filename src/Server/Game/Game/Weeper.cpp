#include "pch.h"
#include "Monster.h"
#include "Weeper.h"
#include "CustomController.h"
#include "MessageHandler.h"
#include "TimeManager.h"
#include "RigidBody.h"
#include "Transform.h"
#include "Collider.h"
#include "CollisionPairInfo.h"
#include "Player.h"
#include "RigidBody.h"

Weeper::Weeper(int32_t MonsterID, const Vec3& position, const Quat& rotation, const Vec3& scale) :
	Monster{ MonsterID, position, rotation, scale },
	m_prevState{ IDLE },
	m_currState{ m_prevState }
{
	m_hp = 20;

	m_name = L"Weeper";
	m_objType = server::OBJECT_TYPE::BOSS;
	m_fbxType = server::FBX_TYPE::WEEPER;
}

Weeper::~Weeper()
{
}

void Weeper::Init()
{
	Monster::Init();
}

void Weeper::Update(double timeDelta)
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

void Weeper::LateUpdate(double timeDelta)
{
	CheckState();
	UpdateFrame();

	Monster::LateUpdate(timeDelta);
}

void Weeper::Release()
{
	Monster::Release();
}

void Weeper::CheckState()
{
	if (m_prevState == m_currState)
		return;

	switch (m_currState)
	{
		case Weeper::CAST1:
		{

		}
		break;
		case Weeper::CAST2_START:
		{

		}
		break;
		case Weeper::CAST2_LOOP:
		{

		}
		break;
		case Weeper::CAST2_END:
		{

		}
		break;
		case Weeper::CAST3:
		{

		}
		break;
		case Weeper::CAST4_START:
		{

		}
		break;
		case Weeper::CAST4_LOOP:
		{

		}
		break;
		case Weeper::CAST4_END:
		{

		}
		break;
		case Weeper::DAMAGE:
		{
		}
		break;
		case Weeper::DEAD:
		{
			SetRemoveReserved();

			game::Message msg{ -1, ProtocolID::WR_REMOVE_ACK };
			msg.objID = m_id;
			msg.objType = m_objType;

			game::MessageHandler::GetInstance()->PushSendMessage(msg);
		}
		break;
		case Weeper::DEATH:
		{
			std::cout << "BOSS DEFEATED!\n";
		}
		break;
		case Weeper::DODGE:
		{

		}
		break;
		case Weeper::IDLE:
		{

		}
		break;
		case Weeper::IDLE_BREAK:
		{

		}
		break;
		case Weeper::STATUE1:
		{

		}
		break;
		case Weeper::STATUE2:
		{

		}
		break;
		case Weeper::STATUE3:
		{

		}
		break;
		case Weeper::TAUNT:
		{

		}
		break;
		case Weeper::TURN_LEFT:
		{

		}
		break;
		case Weeper::WALK:
		{

		}
		break;
		case Weeper::WALK_BACK:
		{

		}
		break;
		case Weeper::WALK_BACK_NO_LEGS:
		{

		}
		break;
		default:
		break;
	}

	m_prevState = m_currState;

	if (m_currState == DEAD)
		return;

	game::Message msg{ -1, ProtocolID::WR_CHANGE_STATE_ACK };
	msg.state = m_currState;
	msg.objID = m_id;
	msg.objType = m_objType;

	game::MessageHandler::GetInstance()->PushSendMessage(msg);
}

void Weeper::UpdateFrame()
{
	switch (m_currState)
	{
		case Weeper::CAST2_LOOP: case Weeper::CAST4_LOOP: case Weeper::IDLE: case Weeper::DEAD:
		case Weeper::STATUE1: case Weeper::STATUE2: case Weeper::STATUE3:
		case Weeper::WALK: case Weeper::WALK_BACK: case Weeper::WALK_BACK_NO_LEGS:
		return;
		default:
		break;
	}

	if (m_aniEnd == true)
	{
		switch (m_currState)
		{
			case Weeper::CAST1: case Weeper::CAST2_END: case Weeper::CAST3: case Weeper::CAST4_END:
			{
				m_currState = Weeper::IDLE;
			}
			break;
			case Weeper::CAST2_START: case Weeper::CAST4_START:
			{
				// CAST2_END로 갈지 CAST2_LOOP로 갈지는 추후 변경
				// 현재는 END로 가도록 설정
				m_currState = magic_enum::enum_value<Weeper::WEEPER_STATE>(m_currState - 2);
			}
			break;
			case Weeper::DEATH:
			{
				m_currState = Weeper::DEAD;
				m_startSendTransform = false;
			}
			break;
			case Weeper::DAMAGE: case Weeper::DODGE: case Weeper::IDLE_BREAK:
			case Weeper::TAUNT: case Weeper::TURN_LEFT: case Weeper::TURN_RIGHT:
			{
				// 추후 각 state 별 추가 사항이 있으면 case 분리
				m_currState = Weeper::IDLE;
			}
			break;
			default:
			break;
		}

		m_aniEnd = false;
	}
}

Weeper::WEEPER_STATE Weeper::GetState() const
{
	return m_currState;
}

void Weeper::SetState(WEEPER_STATE state)
{
	m_currState = state;
}
