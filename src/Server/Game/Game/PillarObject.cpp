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
	m_originalTrans.p = TO_PX3(position);
	m_originalTrans.q = TO_PXQUAT(rotation);
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
	Reset();

	GameObject::Update(timeDelta);
}

void PillarObject::LateUpdate(double timeDelta)
{
	m_body->ClearCollidersCollisionInfo();
	m_transform->ConvertPX(m_body->GetGlobalPose());		//크기정보는 Mesh적용되면 그 때 수정
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
	//m_body->SetAngularDamping(0.0f);
	m_body->SetSleepThresholder(0.02);
}

void PillarObject::ApplyRequestedLayers()
{
	m_body->GetCollider(0)->ApplyModifiedLayer(PhysicsLayers::PILLIAR, PhysicsLayers::NONE);
}

void PillarObject::ReceivedAttack_SingleAttack()
{
	static float currentDampValue = 0.8f;

	currentDampValue -= 0.1f;
	if (currentDampValue < 0.4)
	{
		currentDampValue = 0.3;
		cout << "돌기둥 약화 완료" << endl;
		m_body->SetAngularDamping(0.3f);
	}
	else
	{
		cout << "돌기둥 약화 진행중" << endl;
		cout << "현재 damping값 " << currentDampValue << endl;
	}
}

void PillarObject::ReceivedAttack_Meteor()
{
	float currentDampValue = m_body->GetBody()->getAngularDamping();
	if (currentDampValue > 0.3f)
	{
		m_body->SetVelocity(physx::PxVec3(0));
		auto currentAngVel = m_body->GetBody()->getAngularVelocity();
		m_body->GetBody()->setAngularVelocity(currentAngVel * 0.5f);
		cout << " pilliar : 메테오 충돌 인지완료. damping값 0이 아니다. 조건 불만족." << endl;
		return;
	}
	else
	{
		cout << "pilliar : 메테오 충돌 인지완료. damping값 0이다. 조건 만족. 다리가 연결되어야 한다." << endl;
	}
}

void PillarObject::ServerMessage_Init()
{
	m_id = game::MessageHandler::GetInstance()->NewObjectID();
	m_name = L"PILLAR BRIDGE";
	m_fbxType = server::FBX_TYPE::PILLAR_BRIDGE;
	m_objType = server::OBJECT_TYPE::PHYSX_OBJECT;

	game::TIMER_EVENT ev{ ProtocolID::WR_ADD_OBJ_ACK };
	ev.objType = m_objType;
	ev.objID = m_id;

	//game::MessageHandler::GetInstance()->PushSendMessage(ev);
}

void PillarObject::ServerMessage_Release()
{
	game::TIMER_EVENT ev{ ProtocolID::WR_REMOVE_ACK };
	ev.objID = m_id;
	ev.objType = m_objType;

	game::MessageHandler::GetInstance()->PushSendMessage(ev);
}

void PillarObject::Reset()
{
	static bool wasQDown = false; // static 변수 추가. 이전에 'K'키가 눌려져 있었는지를 기억합니다.
	bool isQDown = GetAsyncKeyState('Q') & 0x8000; // 'K'키가 현재 눌려져 있는지 확인합니다.
	bool ball = false;


	if (wasQDown && !isQDown) // 이전에 'K'키가 눌려져 있었는데, 지금은 떼어져 있다면.
	{
		m_originalTrans;

		m_body->SetRotation(FROM_PXQUAT(m_originalTrans.q));
		m_body->SetPosition(FROM_PX3(m_originalTrans.p), true);

		m_body->SetVelocity(physx::PxVec3(0));
		Init_After_ColliderAttached();
	}

	wasQDown = isQDown; // 'K'키가 눌려져 있는지의 상태를 업데이트합니다.


}

