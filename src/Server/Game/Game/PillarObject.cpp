#include "pch.h"
#include "PillarObject.h"
#include "RigidBody.h"
#include "Transform.h"
#include "TimeManager.h"
#include "MessageHandler.h"
#include "Collider.h"

using namespace std;
using namespace physx;

PillarObject::PillarObject(const Vec3& position, const Quat& rotation, const Vec3& scale)
	: GameObject(position, rotation, scale), m_body(nullptr)
{
}

PillarObject::~PillarObject()
{
}

void PillarObject::Init()
{
	m_body = AddComponent<RigidBody>(L"RigidBody");

	ServerMessage_Init();
}

void PillarObject::Update(double timeDelta)
{

	GameObject::Update(timeDelta);
}

void PillarObject::LateUpdate(double timeDelta)
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

void PillarObject::Release()
{
	m_body = nullptr;
	GameObject::Release();
}

void PillarObject::Init_After_ColliderAttached()
{
	ApplyRequestedLayers();

	m_body->SetKinematic(false);
	m_body->SetRigidBodySleep(false);

	PhysicsAxis axesToLock = static_cast<PhysicsAxis>(static_cast<int>(PhysicsAxis::X) | static_cast<int>(PhysicsAxis::Y));
	m_body->SetRotationLockAxis(axesToLock, true);
	m_body->SetMass(1000.f);
	m_body->SetAngularDamping(0.8f);
	m_body->SetLinearDamping(0.2f);
	m_body->SetSleepThresholder(0.02);
	//m_body->SetSleepThresholder();
}

void PillarObject::ApplyRequestedLayers()
{
	m_body->GetCollider(0)->ApplyModifiedLayer(PhysicsLayers::PILLIAR, PhysicsLayers::NONE);
}

void PillarObject::ReceivedAttack_SingleAttack()
{
	m_body->AddForce(ForceMode::Impulse, physx::PxVec3(1, 0, 0) * 1000.f);
	cout << "singleAttack" << endl;
}

void PillarObject::ReceivedAttack_Ball()
{
	m_body->AddForce(ForceMode::Impulse, physx::PxVec3(1, 0, 0) * 1000.f);
	cout << "Ball" << endl;
}

void PillarObject::ServerMessage_Init()
{
	m_id = game::MessageHandler::GetInstance()->NewObjectID();
	m_name = L"PLAYER FIREBALL";
	m_fbxType = server::FBX_TYPE::PILLAR_BRIDGE;
	m_objType = server::OBJECT_TYPE::PHYSX_OBJECT;

	game::TIMER_EVENT ev{ ProtocolID::WR_ADD_OBJ_ACK };
	ev.objType = m_objType;
	ev.objID = m_id;

	game::MessageHandler::GetInstance()->PushSendMessage(ev);
}

void PillarObject::ServerMessage_Release()
{
	SetRequireFlagTransmit(false);

	game::TIMER_EVENT ev{ ProtocolID::WR_REMOVE_ACK };
	ev.objID = m_id;
	ev.objType = m_objType;

	game::MessageHandler::GetInstance()->PushSendMessage(ev);
}

bool PillarObject::GetRequireFlagTransmit()
{
	return m_requiresPacketTransmit;
}

void PillarObject::SetRequireFlagTransmit(bool set)
{
	m_requiresPacketTransmit = set;
}

