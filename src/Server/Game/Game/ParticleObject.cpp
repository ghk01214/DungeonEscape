﻿#include "pch.h"
#include "ParticleObject.h"
#include "RigidBody.h"
#include "Transform.h"
#include "TimeManager.h"
#include "Collider.h"
#include "MessageHandler.h"
#include "ObjectManager.h"
#include "SphereCollider.h"
#include "EventHandler.h"

ParticleObject::ParticleObject(const Vec3& position, const Quat& rotation, const Vec3& scale)
	: GameObject(position, rotation, scale), m_body(nullptr)
{
}

ParticleObject::~ParticleObject()
{
}

void ParticleObject::Init()
{
	m_body = AddComponent<RigidBody>(L"RigidBody");
	m_body->SetCCDFlag(false);
	m_body->SetKinematic(true);

	//SetObjectType(server::OBJECT_TYPE::MAP_OBJECT);
}

void ParticleObject::Update(double timeDelta)
{
	ServerMessage_Transform();

	GameObject::Update(timeDelta);
}

void ParticleObject::LateUpdate(double timeDelta)
{
	m_body->ClearCollidersCollisionInfo();
	m_transform->ConvertPX(m_body->GetGlobalPose());		//크기정보는 Mesh적용되면 그 때 수정
}

void ParticleObject::Release()
{
	m_body = nullptr;
	GameObject::Release();
}

void ParticleObject::ServerMessage_Init()
{
	m_id = MSG_HANDLER->NewObjectID();
	m_name = L"PARTICLE";
	m_objType = server::OBJECT_TYPE::PARTICLE;

	//std::cout << "create, " << m_id << "\n";
}

void ParticleObject::ServerMessage_Release()
{
	//for (int32_t i = 0; i < SEND_AGAIN; ++i)
	{
		game::TimerEvent ev{ ProtocolID::WR_REMOVE_ACK };
		ev.objID = m_id;
		ev.objType = m_objType;

		MSG_HANDLER->PushSendMessage(ev);
	}

	//std::cout << "release, " << m_id << "\n";
}

void ParticleObject::ServerMessage_Transform()
{
	if (isSleep() == true)
		return;

	game::TimerEvent ev{ ProtocolID::WR_TRANSFORM_ACK };
	ev.objID = m_id;
	ev.objType = m_objType;

	MSG_HANDLER->PushSendMessage(ev);
}

void ParticleObject::ApplyRequestedLayers()
{
	m_body->GetCollider(0)->ApplyModifiedLayer(PhysicsLayers::PARTICLE,
		static_cast<PhysicsLayers>(static_cast<int>(PhysicsLayers::PLAYER) | static_cast<int>(PhysicsLayers::MONSTER)| static_cast<int>(PhysicsLayers::SKILLOBJECT_PLAYER)
			| static_cast<int>(PhysicsLayers::SKILLOBJECT_MONSTER) | static_cast<int>(PhysicsLayers::MAP) | static_cast<int>(PhysicsLayers::PARTICLE)
			));
}

int32_t ParticleObject::Summon(physx::PxVec3 initialPos, float xzInterval, float xzStrength, float yStrength, float deleteTime)
{
	//static std::uniform_real_distribution<float> distributionY(0.f, 1.0f);
	//static std::uniform_real_distribution<float> distributionXZ(-1.f, 1.f);
	//
	//physx::PxVec3 dir = physx::PxVec3(distributionXZ(dre), distributionY(dre), distributionXZ(dre));
	//dir.normalize();
	//dir *= 500.f;
	//dir.y /= 5.f;

	static std::uniform_real_distribution<float> distribution1(-1.f, 1.f);
	static std::uniform_real_distribution<float> distribution2(-1.f, 1.f);


	physx::PxVec3 randomDir = physx::PxVec3(distribution1(dre), 0, distribution1(dre));
	physx::PxVec3 newPos = initialPos + randomDir * xzInterval * xzStrength;
	physx::PxVec3 flyDirection = (newPos - initialPos);
	float yPower = flyDirection.magnitude();
	//std::cout << yPower << std::endl;
	if (yPower < 0)
		yPower *= -1.f;

	float maxValue = sqrt(xzInterval * xzInterval);
				//3이란 값을 수정하
	flyDirection.y = maxValue - yPower + maxValue * yStrength;
	//(initial - newpos)기반 날려보내기



	auto objmgr = ObjectManager::GetInstance();
	auto particleObj = objmgr->AddGameObjectToLayer<ParticleObject>(L"Layer_Particle", FROM_PX3(initialPos), Quat(0, 0, 0, 1), Vec3(500, 500, 500));
	auto particleBody = particleObj->GetComponent<RigidBody>(L"RigidBody");
	particleBody->AddCollider<SphereCollider>(Vec3(100, 100, 100));
	particleObj->ApplyRequestedLayers();

	particleBody->SetKinematic(false);
	particleBody->SetRigidBodySleep(false);

	particleBody->AddForce(ForceMode::Impulse, flyDirection);


	particleObj->ServerMessage_Init();
	EventHandler::GetInstance()->AddEvent("REMOVE_PARTICLE", deleteTime, particleObj);
	//EventHandler::GetInstance()->AddEvent("STOP_PARTICLE", deleteTime, particleObj);

	return particleObj->GetID();
}