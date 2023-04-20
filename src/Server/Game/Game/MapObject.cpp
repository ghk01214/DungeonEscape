#include "pch.h"
#include "MapObject.h"
#include "RigidBody.h"
//#include "BoxCollider.h"
//#include "SphereCollider.h"
//#include "MeshCollider.h"
#include "Transform.h"

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
}

void MapObject::Update(double timeDelta)
{
}

void MapObject::LateUpdate(double timeDelta)
{
}

void MapObject::Release()
{
	m_body = nullptr;
	GameObject::Release();
}
