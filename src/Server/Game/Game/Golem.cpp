#include "pch.h"
#include "Golem.h"
#include "GolemAI.h"
#include "SkillObject.h"
#include "CustomController.h"
#include "MessageHandler.h"
#include "TimeManager.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
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
#include "OverlapObject.h"

Golem::Golem(int32_t MonsterID, const Vec3& position, const Quat& rotation, const Vec3& scale) :
	Monster{ MonsterID, position, rotation, scale },
	m_prevState{ IDLE1 },
	m_currState{ m_prevState },
	m_sendState{ 0 }
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
	m_hp = 1000;
	m_maxHp = 1000;

	m_controller->GetBody()->SetMass(1000.f);
	//SetUp_PatternObjects();

	m_golemAI = new GolemAI(this);
	m_golemAI->Init();

	m_AI = m_golemAI;
	m_overlapObject = new OverlapObject(m_AI);
}

void Golem::Update(double timeDelta)
{
	SendChangedStateAgain();

	m_golemAI->Update(timeDelta);
	if(m_overlapObject)
		m_overlapObject->Update();

	CheckState();				//State를 진입할 때
	UpdateFrameOnce();			//State를 떠날때
	UpdateFrameRepeat();		//State에 있을 때

	Monster::Update(timeDelta);
}

void Golem::LateUpdate(double timeDelta)
{
	Monster::LateUpdate(timeDelta);
}

void Golem::Release()
{
	SafeRelease(m_overlapObject);
	SafeRelease(m_golemAI);


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
		}
		break;
		case DEATH:
		{
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
		case WALK:
		{

		}
		break;
		default:
		break;
	}

	m_prevState = m_currState;
	m_sendState = SEND_AGAIN;

	game::TIMER_EVENT ev{ ProtocolID::WR_CHANGE_STATE_ACK };
	ev.state = m_currState;
	ev.objID = m_id;
	ev.objType = m_objType;

	game::MessageHandler::GetInstance()->PushSendMessage(ev);
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

void Golem::UpdateFrameOnce()
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
			m_golemAI->UpdateTargetPos();		//패턴이 끝났으면 바로 플레이어를 바라보도록
		}
		break;
		case SPELL_START: case JUMP_START:
		{
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

GolemAI* Golem::GetAI()
{
	return m_golemAI;
}

//void Golem::SetUp_PatternObject(GOLEM_SCHEDULE schedulename, GeometryType geometry, float startTime, float endTime, Vec3 patternSize, TriggerObject::SKILLTYPE type)
//{
//	auto objmgr = ObjectManager::GetInstance();
//	TriggerObject* attackTrigger = objmgr->AddGameObjectToLayer<TriggerObject>(L"Layer_TriggerObject", Vec3(0,-10000, 0), Quat(0,0,0,0), patternSize, type);		//생성
//	attackTrigger->SetTriggerType(server::TRIGGER_TYPE::SINGLE_STRIKE, startTime, endTime);								//설정 : 단일타, 시작시간, 끝시간
//	auto body = attackTrigger->GetComponent<RigidBody>(L"RigidBody");																					
//	
//	if(geometry == GeometryType::Box)																					//콜라이더 부착
//		body->AddCollider<BoxCollider>(attackTrigger->GetTransform()->GetScale());
//	else if (geometry == GeometryType::Sphere)
//		body->AddCollider<SphereCollider>(attackTrigger->GetTransform()->GetScale());
//	
//	body->GetCollider(0)->SetTrigger(true);																				//트리거 설정 참
//
//	m_patternTriggerDict[schedulename] = attackTrigger;
//}

//void Golem::SetUp_PatternObjects()
//{
//	SetUp_PatternObject(GOLEM_SCHEDULE::ATTACK1, GeometryType::Box, 0.3f, 1.5f, Vec3(200, 200, 300), TriggerObject::SKILLTYPE::GOLEM_ATTACK1);
//	SetUp_PatternObject(GOLEM_SCHEDULE::ATTACK2, GeometryType::Box, 0.3f, 1.53f, Vec3(200, 400, 400), TriggerObject::SKILLTYPE::GOLEM_ATTACK2);
//}

void Golem::Pattern_Attack1()
{
	//AddSkillSize에서 줬던 이름으로
	if(m_overlapObject)
		m_overlapObject->Activate("ATTACK1");

}

void Golem::Pattern_Attack2()
{
	if (m_overlapObject)
		m_overlapObject->Activate("ATTACK2");
}

void Golem::Pattern_Attack3()
{
	if (m_overlapObject)
		m_overlapObject->Activate("ATTACK3");
}

void Golem::Pattern_Attack4()
{
	if (m_overlapObject)
		m_overlapObject->Activate("ATTACK4");
}

void Golem::Pattern_Roar()
{
	if (m_overlapObject)
		m_overlapObject->Activate("ROAR");
}

void Golem::Pattern_Run()
{
	if (m_overlapObject)
		m_overlapObject->Activate("RUN");
}

void Golem::OverlapObject_Deactivate()
{
	//AddSkillSize에서 줬던 이름으로
	if (m_overlapObject)
		m_overlapObject->Deactivate();
}



Golem::GOLEM_STATE Golem::GetState() const
{
	return m_currState;
}

void Golem::SetState(GOLEM_STATE state)
{
	m_currState = state;
}

void Golem::SendChangedStateAgain()
{
	if (m_sendState <= 0)
		return;

	game::TIMER_EVENT ev{ ProtocolID::WR_CHANGE_STATE_ACK, m_id };
	ev.state = m_currState;
	ev.objType = m_objType;

	game::MessageHandler::GetInstance()->PushSendMessage(ev);

	--m_sendState;
}
