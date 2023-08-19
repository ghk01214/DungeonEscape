#include "pch.h"
#include "ArtifactObject.h"
#include "RigidBody.h"
#include "Transform.h"
#include "TimeManager.h"
#include "MessageHandler.h"
#include "EventHandler.h"
#include "ObjectManager.h"
#include "Collider.h"
#include "RigidBody.h"
#include "PillarObject.h"

using namespace std;
using namespace physx;

ArtifactObject::ArtifactObject(const Vec3& position, const Quat& rotation, const Vec3& scale)
	: GameObject(position, rotation, scale), m_body(nullptr)
{
}

ArtifactObject::~ArtifactObject()
{
}

void ArtifactObject::Init()
{
	m_body = AddComponent<RigidBody>(L"RigidBody");

	ServerMessage_Init();
}

void ArtifactObject::Update(double timeDelta)
{
	GameObject::Update(timeDelta);
}

void ArtifactObject::LateUpdate(double timeDelta)
{
	m_body->ClearCollidersCollisionInfo();
	m_transform->ConvertPX(m_body->GetGlobalPose());		//크기정보는 Mesh적용되면 그 때 수정
}

void ArtifactObject::Release()
{
	m_body = nullptr;
	GameObject::Release();
}

void ArtifactObject::ApplyRequestedLayers()
{
	m_body->GetCollider(0)->ApplyModifiedLayer(PhysicsLayers::MAP, PhysicsLayers::NONE);
}

void ArtifactObject::ReceivedAttack_SingleAttack()
{
	static int requestedHit = 5;
	static bool once = true;

	if (requestedHit > 0)
	{
		requestedHit -= 1;
		cout << "아티팩트 약화 완료 : " << requestedHit <<endl;
	}
	else
	{
		if (once)
		{
			//HERE
			cout << "아티팩트 무력화 완료" << endl;
			SetPillarWeak();
			ServerFunctionHere();
			once = false;
			SetRemoveReserved();
		}
	}
}

void ArtifactObject::SetPillarWeak()
{
	auto list = ObjectManager::GetInstance()->GetLayer(L"Layer_Gimmik_Pillar")->GetGameObjects();
	for (auto& p : list)
	{
		//p->GetComponent<RigidBody>(L"RigidBody")->SetKinematic(true);
		p->GetComponent<RigidBody>(L"RigidBody")->SetAngularDamping(0.3f);
	}
}


void ArtifactObject::ServerMessage_Init()
{
	//m_id = game::MessageHandler::GetInstance()->NewObjectID();
	m_name = L"ARTIFACT";				// 이름은 이걸로 고정. 해당 코드라인은 수정X
	//m_fbxType = server::FBX_TYPE::PILLAR_BRIDGE;
	//m_objType = server::OBJECT_TYPE::PHYSX_OBJECT;
	//
	//game::TIMER_EVENT ev{ ProtocolID::WR_ADD_OBJ_ACK };
	//ev.objType = m_objType;
	//ev.objID = m_id;
}

void ArtifactObject::ServerMessage_Release()
{
	//game::TIMER_EVENT ev{ ProtocolID::WR_REMOVE_ACK };
	//ev.objID = m_id;
	//ev.objType = m_objType;
	//
	//game::MessageHandler::GetInstance()->PushSendMessage(ev);
}

void ArtifactObject::ServerFunctionHere()
{
	// * 아티팩트 무력화가 되는 순간 호출되는 함수이다.
	// 1. 무력화 되는 순간 컷씬을 전환해 Pillar를 비춘다.
	// 2. 아티팩트 무력화가 되는 순간 4초 후 기둥에서 마법이펙트가 꺼지는걸 보여준다. (카메라 컷씬 전환되는 시간이 있으니, 4초 후 재생)
	// 3. Pillar에서 마법 이펙트가 완전히 사라지면 잠시 있다 컷씬을 종료해 카메라를 원래대로 되돌린다.


	// 페이드인/아웃 + 카메라 전환 명령 코드 작성										(1)
	game::TIMER_EVENT ev{ ProtocolID::WR_PLAY_CUT_SCENE_ACK };
	ev.integer = magic_enum::enum_integer(server::CUT_SCENE_TYPE::SCENE1);
	ev.objID = 0;

	game::MessageHandler::GetInstance()->PushSendMessage(ev);

	//EventHandler::GetInstance()->AddEvent("PILLAR_EFFECT_OFF", 4.f, this);			//	(2)
	//EventHandler::GetInstance()->AddEvent("CAMERA_BACKTO_PLAYER", 8.f, this);	//	(3) 3이 따로 필요없다면 Event.h에서 CAMERA_BACKTO_PLAYER를 삭제.

	// AddEvent함수는 원하는 타이밍에 어떤 행동을 할 수 있게 해준다.
	// 클라이언트에서 스스로 컷씬 시간이나 이펙트 제거 시간등을 제어할 수 있다면 AddEvent()는 굳이 사용할 필요는 없다.
	// 필요없다면 Event.cpp에서 PILLAR_EFFECT_OFF, CAMERA_BACKTO_PLAYER가 적힌 if문을 삭제
	// 스킬 사라지는건 투명도 조절하여 없앨것.
}

