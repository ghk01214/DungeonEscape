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

	if (m_body->IsRigidbodySleep())
	{
		m_requiresPacketTransmit = false;
	}
	else
	{	//위치 갱신
		m_transform->ConvertPX(m_body->GetGlobalPose());		//크기정보는 Mesh적용되면 그 때 수정
		m_requiresPacketTransmit = true;						//위치, 회전정보만 갱신.
	}
}

void MapObject::Release()
{
	m_body = nullptr;
	GameObject::Release();
}

bool MapObject::GetRequireFlagTransmit()
{
	return m_requiresPacketTransmit;
}

void MapObject::SetRequireFlagTransmit(bool value)
{
	m_requiresPacketTransmit = value;
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
}

void MapObject::ServerMessage_Release()
{
	SetRequireFlagTransmit(false);

	game::TIMER_EVENT ev{ ProtocolID::WR_REMOVE_ACK };
	ev.objID = m_id;
	ev.objType = m_objType;

	game::MessageHandler::GetInstance()->PushSendMessage(ev);
}

void MapObject::ApplyRequestedLayers()
{
	m_body->GetCollider(0)->ApplyModifiedLayer(PhysicsLayers::MAP, PhysicsLayers::NONE);
}

