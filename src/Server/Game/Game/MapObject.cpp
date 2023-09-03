#include "pch.h"
#include "MapObject.h"
#include "RigidBody.h"
//#include "BoxCollider.h"
//#include "SphereCollider.h"
//#include "MeshCollider.h"
#include "Transform.h"
#include "TimeManager.h"
#include "Collider.h"
#include "MessageHandler.h"
#include "ObjectManager.h"
#include "Golem.h"

MapObject::MapObject(const Vec3& position, const Quat& rotation, const Vec3& scale)
	: GameObject(position, rotation, scale), m_body(nullptr)
{
}

MapObject::~MapObject()
{
}

void MapObject::Init()
{
	m_body = AddComponent<RigidBody>(L"RigidBody");
	m_body->SetCCDFlag(false);
	m_body->SetKinematic(true);

	SetObjectType(server::OBJECT_TYPE::MAP_OBJECT);
}

void MapObject::Update(double timeDelta)
{
	//auto collider = m_body->GetCollider(0);
	//if (collider)
	//{
	//	if (m_body->GetCollider(0)->IsTrigger())
	//	{
	//		m_body->SetRigidBodySleep(false);
	//	}
	//}


	GameObject::Update(timeDelta);
}

void MapObject::LateUpdate(double timeDelta)
{
	m_body->ClearCollidersCollisionInfo();
	m_transform->ConvertPX(m_body->GetGlobalPose());		//크기정보는 Mesh적용되면 그 때 수정
}

void MapObject::Release()
{
	m_body = nullptr;
	GameObject::Release();
}

void MapObject::RecordInitialPosition(Vec3 position)
{
	m_initialPosition = position;
	m_riseupPosition = position;
	m_riseupPosition.y += BOSSROCKINTERVAL;
}

bool MapObject::SinkBelow()
{
	if (m_skip)						//골렘이 위에 있어서 riseup이 호출되지 않았던놈이면 (아예 올라오질 않았다면)
		return false;				//올라오지 마라

	physx::PxVec3 curPos = m_body->GetPosition();

	if (abs(abs(curPos.y) - abs(m_riseupPosition.y)) > BOSSROCKINTERVAL)
	{
		std::cout << "BossRock sinked to original Position" << std::endl;
		return true;		//지정위치까지 이동했다면 true를 리턴
	}

	curPos.y -= BOSSROCKSPEED;
	m_body->SetPosition(FROM_PX3(curPos), true);
	return false;
}

bool MapObject::RiseUp()
{
	CheckSkip();					//골렘이 위에 있다면
	if (m_skip)						
		return false;				//올라오지 마라

	physx::PxVec3 curPos = m_body->GetPosition();

	if (abs(abs(curPos.y) - abs(m_riseupPosition.y)) < 30.f)
	{
		std::cout << "BossRock rised to original Position" << std::endl;
		return true;		//지정위치까지 이동했다면 true를 리턴
	}

	curPos.y += BOSSROCKSPEED;
	m_body->SetPosition(FROM_PX3(curPos), true);
	return false;
}

void MapObject::CheckSkip()
{
	auto golem = ObjectManager::GetInstance()->GetLayer(L"Layer_Monster")->GetGameObjectByName<Golem>(L"Golem");
	if (!golem)
		return;


	physx::PxVec3 curPos = m_body->GetPosition();
	curPos.y = 0;

	Vec3 golemPos = golem->GetControllerPosition();
	physx::PxVec3 pxGolemPos = TO_PX3(golemPos);
	pxGolemPos.y = 0;

	physx::PxVec3 diff = curPos - pxGolemPos;
	if (diff.magnitude() < 700)
	{
		m_skip = true;
		return;
	}
}

void MapObject::SkipClear()
{
	m_skip = false;
}

void MapObject::ServerMessage_Init(bool scatterRock, bool boulder)
{
	if (scatterRock)
	{										//기믹 돌담
		m_id = game::MessageHandler::GetInstance()->NewObjectID();

		m_name = L"BLOCK";
		m_fbxType = server::FBX_TYPE::SCATTER_ROCK;
		m_objType = server::OBJECT_TYPE::MAP_OBJECT;

		game::TIMER_EVENT ev{ ProtocolID::WR_ADD_OBJ_ACK };
		ev.objType = m_objType;
		ev.objID = m_id;

		game::MessageHandler::GetInstance()->PushSendMessage(ev);
	}
	else if (boulder)
	{										//기믹 공
		m_id = game::MessageHandler::GetInstance()->NewObjectID();

		m_name = L"ROLLING ROCK";
		m_fbxType = server::FBX_TYPE::ROLLING_ROCK;
		m_objType = server::OBJECT_TYPE::MAP_OBJECT;

		game::TIMER_EVENT ev{ ProtocolID::WR_ADD_OBJ_ACK };
		ev.objType = m_objType;
		ev.objID = m_id;

		game::MessageHandler::GetInstance()->PushSendMessage(ev);
	}
	else
	{			// 골렘 방 4개 돌
		m_id = game::MessageHandler::GetInstance()->NewObjectID();

		m_name = L"LAST BOSS ROCK";
		m_fbxType = server::FBX_TYPE::LAST_BOSS_ROCK;
		m_objType = server::OBJECT_TYPE::MAP_OBJECT;

		game::TIMER_EVENT ev{ ProtocolID::WR_ADD_OBJ_ACK };
		ev.objType = m_objType;
		ev.objID = m_id;

		game::MessageHandler::GetInstance()->PushSendMessage(ev);
	}
}

void MapObject::ServerMessage_Release()
{
	game::TIMER_EVENT ev{ ProtocolID::WR_REMOVE_ACK };
	ev.objID = m_id;
	ev.objType = m_objType;

	game::MessageHandler::GetInstance()->PushSendMessage(ev);
}

void MapObject::ApplyRequestedLayers()
{
	m_body->GetCollider(0)->ApplyModifiedLayer(PhysicsLayers::MAP, PhysicsLayers::NONE);
}

