﻿#include "pch.h"
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
	SetUp_PatternObjects();

	m_golemAI = new GolemAI(this);
	m_golemAI->Init();
	m_AI = m_golemAI;
}

void Golem::Update(double timeDelta)
{
	SendChangedStateAgain();

	m_golemAI->Update(timeDelta);

	CheckState();
	UpdateFrame();
	UpdateFrameRepeat();

	Monster::Update(timeDelta);
}

void Golem::LateUpdate(double timeDelta)
{
	Monster::LateUpdate(timeDelta);
}

void Golem::Release()
{
	SafeRelease(m_golemAI);

	for (auto& triggerObj : m_patternTriggerDict)
	{
		triggerObj.second->GetRemoveReserved();
		m_patternTriggerDict[triggerObj.first] = nullptr;
	}

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
			m_golemAI->UpdateTargetPos();		//패턴이 끝났으면 바로 플레이어를 바라보도록
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

GolemAI* Golem::GetAI()
{
	return m_golemAI;
}

void Golem::SetUp_PatternObject(GOLEM_SCHEDULE schedulename, GeometryType geometry, float startTime, float endTime, Vec3 patternSize, TriggerObject::SKILLTYPE type)
{
	auto objmgr = ObjectManager::GetInstance();
	TriggerObject* attackTrigger = objmgr->AddGameObjectToLayer<TriggerObject>(L"Layer_TriggerObject", Vec3(0,-10000, 0), Quat(0,0,0,0), patternSize, type);		//생성
	attackTrigger->SetTriggerType(server::TRIGGER_TYPE::SINGLE_STRIKE, startTime, endTime);								//설정 : 단일타, 시작시간, 끝시간
	auto body = attackTrigger->GetComponent<RigidBody>(L"RigidBody");																					
	
	if(geometry == GeometryType::Box)																					//콜라이더 부착
		body->AddCollider<BoxCollider>(attackTrigger->GetTransform()->GetScale());
	else if (geometry == GeometryType::Sphere)
		body->AddCollider<SphereCollider>(attackTrigger->GetTransform()->GetScale());
	
	body->GetCollider(0)->SetTrigger(true);																				//트리거 설정 참

	m_patternTriggerDict[schedulename] = attackTrigger;
}

void Golem::SetUp_PatternObjects()
{
	SetUp_PatternObject(GOLEM_SCHEDULE::ATTACK1, GeometryType::Box, 0.3f, 1.5f, Vec3(200, 200, 300), TriggerObject::SKILLTYPE::GOLEM_ATTACK1);
	SetUp_PatternObject(GOLEM_SCHEDULE::ATTACK2, GeometryType::Box, 0.3f, 1.53f, Vec3(200, 400, 400), TriggerObject::SKILLTYPE::GOLEM_ATTACK2);
}

physx::PxQuat Golem::GetRotation_For_Pattern(physx::PxVec3 xzDir)
{
	physx::PxVec3 initialDir(0, 0, 1);
	physx::PxVec3 rotAxis = initialDir.cross(xzDir).getNormalized();
	physx::PxReal rotAngle = physx::PxAtan2(physx::PxSqrt(1 - physx::PxPow(xzDir.dot(initialDir), 2)), xzDir.dot(initialDir));
	physx::PxQuat rotation(rotAngle, rotAxis);

	return rotation;
}

void Golem::Pattern_Attack1()
{
	m_golemAI->UpdateTargetPos();
	physx::PxVec3 xzDir = m_golemAI->GetXZDir();

	auto triggerObj = m_patternTriggerDict[GOLEM_SCHEDULE::ATTACK1];
	auto triggerBody = triggerObj->GetComponent<RigidBody>(L"RigidBody");
	auto shape = triggerBody->GetCollider(0)->GetPxShape();

	triggerObj->RestoreOneTimeEffect();									//Exclude 해제, 시간 설정 초기화

	//위치, 회전값 적용
	physx::PxVec3 golemPos = TO_PX3(GetControllerPosition());
	physx::PxQuat rotation = GetRotation_For_Pattern(xzDir);

	physx::PxGeometryType::Enum type = shape->getGeometryType();
	if (type == physx::PxGeometryType::eBOX)
	{
		float halfExtentZ = triggerObj->GetTransform()->GetScale().z / 2.f;
		float controllerRadius = m_controller->GetCollider()->GetRadius();

		//회전과 박스를 고려한 적당한 위치에 생성 (캐릭터 앞에 박스 생성하도록)
		auto triggerTrans = m_patternTriggerDict[GOLEM_SCHEDULE::ATTACK1]->GetTransform();
		Vec3 adjustedPos = FROM_PX3(golemPos) + FROM_PX3(xzDir) * (controllerRadius + halfExtentZ);

		triggerBody->SetPosition(adjustedPos, true);
		triggerBody->SetRotation(FROM_PXQUAT(rotation));
	}

	else if (type == physx::PxGeometryType::eSPHERE)
	{
		auto triggerTrans = m_patternTriggerDict[GOLEM_SCHEDULE::ATTACK1]->GetTransform();

		triggerBody->SetPosition(FROM_PX3(golemPos), true);
		triggerBody->SetRotation(FROM_PXQUAT(rotation));
	}
}

void Golem::Pattern_Attack2()
{
	m_golemAI->UpdateTargetPos();
	physx::PxVec3 xzDir = m_golemAI->GetXZDir();

	auto triggerObj = m_patternTriggerDict[GOLEM_SCHEDULE::ATTACK2];
	auto triggerBody = triggerObj->GetComponent<RigidBody>(L"RigidBody");
	auto shape = triggerBody->GetCollider(0)->GetPxShape();

	triggerObj->RestoreOneTimeEffect();									//Exclude 해제, 시간 설정 초기화

	//위치, 회전값 적용
	physx::PxVec3 golemPos = TO_PX3(GetControllerPosition());
	physx::PxQuat rotation = GetRotation_For_Pattern(xzDir);

	physx::PxGeometryType::Enum type = shape->getGeometryType();
	if (type == physx::PxGeometryType::eBOX)
	{
		float halfExtentZ = triggerObj->GetTransform()->GetScale().z / 2.f;
		float controllerRadius = m_controller->GetCollider()->GetRadius();

		//회전과 박스를 고려한 적당한 위치에 생성 (캐릭터 앞에 박스 생성하도록)
		auto triggerTrans = m_patternTriggerDict[GOLEM_SCHEDULE::ATTACK2]->GetTransform();
		Vec3 adjustedPos = FROM_PX3(golemPos) + FROM_PX3(xzDir) * (controllerRadius + halfExtentZ);

		triggerBody->SetPosition(adjustedPos, true);
		triggerBody->SetRotation(FROM_PXQUAT(rotation));
	}

	else if (type == physx::PxGeometryType::eSPHERE)
	{
		auto triggerTrans = m_patternTriggerDict[GOLEM_SCHEDULE::ATTACK2]->GetTransform();

		triggerBody->SetPosition(FROM_PX3(golemPos), true);
		triggerBody->SetRotation(FROM_PXQUAT(rotation));
	}
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
