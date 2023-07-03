#include "pch.h"
#include "Weeper.h"
#include "WeeperAI.h"
#include "SkillObject.h"
#include "CustomController.h"
#include "MessageHandler.h"
#include "TimeManager.h"
#include "RigidBody.h"
#include "Transform.h"
#include "Collider.h"
#include "CollisionPairInfo.h"
#include "CapsuleCollider.h"
#include "ObjectManager.h"
#include "Layer.h"
#include "Player.h"
#include "RigidBody.h"
#include "MonsterAI.h"
#include "EventHandler.h"

Weeper::Weeper(int32_t MonsterID, const Vec3& position, const Quat& rotation, const Vec3& scale) :
	Monster{ MonsterID, position, rotation, scale },
	m_prevState{ IDLE },
	m_currState{ m_prevState }
{
	m_hp = 20;

	m_name = L"Weeper";
	m_objType = server::OBJECT_TYPE::BOSS;
	m_fbxType = server::FBX_TYPE::WEEPER1;
}

Weeper::~Weeper()
{
}

void Weeper::Init()
{
	Monster::Init();

	m_AI = new WeeperAI(this);
	m_AI->Init();
}

void Weeper::Update(double timeDelta)
{
	m_AI->Update(timeDelta);

	CheckState();
	UpdateFrame();

	Monster::Update(timeDelta);
}

void Weeper::LateUpdate(double timeDelta)
{
	Monster::LateUpdate(timeDelta);
}

void Weeper::Release()
{
	Monster::Release();

	SafeRelease(m_AI);
}

void Weeper::CheckState()
{
	if (m_prevState == m_currState)
		return;

	switch (m_currState)
	{
		case CAST1:
		{

		}
		break;
		case CAST2_START:
		{

		}
		break;
		case CAST2_LOOP:
		{

		}
		break;
		case CAST2_END:
		{

		}
		break;
		case CAST3:
		{

		}
		break;
		case CAST4_START:
		{

		}
		break;
		case CAST4_LOOP:
		{

		}
		break;
		case CAST4_END:
		{

		}
		break;
		case DAMAGE:
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
		case DODGE:
		{

		}
		break;
		case IDLE:
		{

		}
		break;
		case IDLE_BREAK:
		{

		}
		break;
		case STATUE1:
		{

		}
		break;
		case STATUE2:
		{

		}
		break;
		case STATUE3:
		{

		}
		break;
		case TAUNT:
		{

		}
		break;
		case TURN_LEFT:
		{

		}
		break;
		case WALK:
		{

		}
		break;
		case WALK_BACK:
		{

		}
		break;
		case WALK_BACK_NO_LEGS:
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

void Weeper::UpdateFrame()
{
	switch (m_currState)
	{
		case CAST2_LOOP: case CAST4_LOOP: case IDLE: case DEAD:
		case STATUE1: case STATUE2: case STATUE3:
		case WALK: case WALK_BACK: case WALK_BACK_NO_LEGS:
		return;
		default:
		break;
	}

	if (m_aniEnd == false)
		return;

	switch (m_currState)
	{
		case CAST1: case CAST2_END: case CAST3: case CAST4_END:
		{
			m_currState = IDLE;
		}
		break;
		case CAST2_START: case CAST4_START:
		{
			// CAST2_END로 갈지 CAST2_LOOP로 갈지는 추후 변경
			// 현재는 END로 가도록 설정
			m_currState = magic_enum::enum_value<WEEPER_STATE>(m_currState - 2);
		}
		break;
		case DEATH:
		{
			m_currState = DEAD;
			m_startSendTransform = false;
		}
		break;
		case DAMAGE: case DODGE: case IDLE_BREAK:
		case TAUNT: case TURN_LEFT: case TURN_RIGHT:
		{
			// 추후 각 state 별 추가 사항이 있으면 case 분리
			m_currState = IDLE;
		}
		break;
		default:
		break;
	}

	m_aniEnd = false;
}

void Weeper::Pattern_Cast1()
{
	//투사체 위치 선정
	physx::PxVec3 monsterPos = m_controller->GetBody()->GetGlobalPose().p;
	monsterPos.y += 200.f;		//weeper 모델 위치 고려해서 살짝 위로
	physx::PxVec3 lookDir = TO_PX3(m_AI->m_targetDir);
	lookDir.getNormalized();

	float monsterRadius = m_controller->GetCollider()->GetRadius();
	float skillBallHalfExtent = 100.f;

	physx::PxVec3 skillBallPosition = monsterPos + lookDir * (monsterRadius + skillBallHalfExtent + 10);
	Vec3 ballPos = FROM_PX3(skillBallPosition);

	SkillObject::SKILLOBJECTTYPE skilltype = SkillObject::SKILLOBJECTTYPE::WEEPER_CAST1_BALL;

	//투사체 생성
	auto objmgr = ObjectManager::GetInstance();
	auto layer = objmgr->GetLayer(L"Layer_SkillObject");
	auto skillObject = objmgr->AddGameObjectToLayer<SkillObject>
		(L"Layer_SkillObject", ballPos, Quat(0, 0, 0, 1), Vec3(skillBallHalfExtent, skillBallHalfExtent, skillBallHalfExtent), skilltype, m_AI->m_target);
	
	EventHandler::GetInstance()->AddEvent("SKILL_GUIDESTART", 5.f, skillObject);		//5초후 추적시작
}

Weeper::WEEPER_STATE Weeper::GetState() const
{
	return m_currState;
}

void Weeper::SetState(WEEPER_STATE state)
{
	m_currState = state;
}
