#include "pch.h"
#include "MapObject.h"
#include "RigidBody.h"
//#include "BoxCollider.h"
//#include "SphereCollider.h"
//#include "MeshCollider.h"
#include "Transform.h"
#include "TimeManager.h"
#include "Collider.h"

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

	if (!m_body->IsRigidbodySleep())
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

void MapObject::ApplyRequestedLayers()
{
	m_body->GetCollider(0)->ApplyModifiedLayer(PhysicsLayers::MAP, PhysicsLayers::NONE);
}

