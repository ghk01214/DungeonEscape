#include "pch.h"
#include "TriggerObject2.h"
#include "PhysDevice.h"
#include "physx_utils.h"
#include "RigidBody.h"
#include "Collider.h"
#include "ObjectManager.h"
#include "Player.h"
#include "Transform.h"
#include "EventHandler.h"
#include "MessageHandler.h"
#include "SphereCollider.h"
#include "Weeper.h"
#include "Golem.h"

using namespace physx;
using namespace std;

TriggerObject2::TriggerObject2(const Vec3& position, const Quat& rotation, const Vec3& scale, bool once)
	: MapObject(position, rotation, scale), m_oneTimeOnly(once)
{
}

TriggerObject2::~TriggerObject2()
{
}

void TriggerObject2::Init()
{
	MapObject::Init();
	SetObjectType(server::OBJECT_TYPE::TRIGGER_OBJECT2);
	m_body->ExcludeFromSimulation(true);

	GeometryInit(m_transform->GetScale() / 2.f);

	ServerInit();
	SetPortalDestination();
}

void TriggerObject2::Update(double timeDelta)
{
	MapObject::Update(timeDelta);				//removeReserved 체크

	if (m_deactivate)
		return;

	if (m_attribute == TRIGGERATTRIBUTE::GUIDELINE1)
		CheckArtifactDestoryed();

	Handle_Overlap();
	AttributePortal(timeDelta);
}

void TriggerObject2::LateUpdate(double timeDelta)
{
	MapObject::LateUpdate(timeDelta);			//collisionInfo 비우기
}

void TriggerObject2::Release()
{
	SafeDelete(m_box);
	MapObject::Release();
}

void TriggerObject2::SetTriggerAttribute(TRIGGERATTRIBUTE attrib)
{
	m_attribute = attrib;
}

void TriggerObject2::GeometryInit(Vec3 Scale)
{
	m_box = new physx::PxBoxGeometry(TO_PX3(Scale));
	if (!m_box->isValid())
		return;
}

void TriggerObject2::Handle_Overlap()
{
	std::vector<Player*> validPlayers = OverlapCheck_Player();		//overlap진행

	Handle_OverlapOut(validPlayers);

	for (auto& player : validPlayers)
	{
		bool duplicate = IsPlayerDuplicate(player);					//중복체크
		if (duplicate)
			continue;
		else
		{
			Apply(player);				//진입
			m_duplicates.emplace_back(player);
		}
	}

	// HERE : 인원수에 변화가 생길 경우 출력.
	m_currentCnt = m_duplicates.size();
	if (m_currentCnt != m_beforeCnt)
	{
		m_beforeCnt = m_currentCnt;

		if (m_attribute < TRIGGERATTRIBUTE::GUIDELINE1)
		{
			for (int32_t i = 0; i < SEND_AGAIN; ++i)
			{
				ServerSendInteractionCountMessage();
			}
		}

		if (m_attribute == TRIGGERATTRIBUTE::CUTSCENE4)
			AttributeGimmik2();

		if (m_attribute == TRIGGERATTRIBUTE::CUTSCENE5)
			Attribute_BossCutscene_0();

		if (m_attribute == TRIGGERATTRIBUTE::CUTSCENE6)
			Attribute_BossCutscene_1();
	}
}

void TriggerObject2::SetPortalDestination()
{
	//HERE
	m_portalDestination.resize(static_cast<int>(TRIGGERATTRIBUTE::END));

	m_portalDestination[static_cast<int>(TRIGGERATTRIBUTE::PORTAL1)] = PORTAL1_EXIT;
	m_portalDestination[static_cast<int>(TRIGGERATTRIBUTE::PORTAL2)] = PORTAL2_EXIT;
	m_portalDestination[static_cast<int>(TRIGGERATTRIBUTE::PORTAL3)] = PORTAL3_EXIT;
	m_portalDestination[static_cast<int>(TRIGGERATTRIBUTE::PORTAL4)] = PORTAL4_EXIT;
	m_portalDestination[static_cast<int>(TRIGGERATTRIBUTE::PORTAL5)] = PORTAL5_EXIT;

	//여기서 포탈 1, 포탈2, 등의 이동 위치를 설정한다.
}

void TriggerObject2::AttributePortal(double timeDelta)
{
	int attribNum = static_cast<int>(m_attribute);
	if (attribNum < static_cast<int>(TRIGGERATTRIBUTE::PORTAL1) || attribNum > static_cast<int>(TRIGGERATTRIBUTE::PORTAL8))
		return;

	string name = string(magic_enum::enum_name(m_attribute));

	if (m_duplicates.size() < 2)
	//if (m_duplicates.empty())
	{
		EventHandler::GetInstance()->DeleteEvent(name);
		return;
	}
	else if (m_duplicates.size() == 2)
	{
		EventHandler::GetInstance()->AddEventIfNone(name, m_requestedContactTime, this);
		return;
	}


	//EventHandler::GetInstance()->AddEventIfNone("TRIGGERCLOCK")
}

void TriggerObject2::Attribute_BossCutscene_0()
{
	if (m_attribute != TRIGGERATTRIBUTE::CUTSCENE5)
		return;


	auto monsterlist = ObjectManager::GetInstance()->GetLayer(L"Layer_Monster")->GetGameObjects();
	if (monsterlist.empty())
		return;

	for (auto& m : monsterlist)
	{
		if (m->GetName() == L"Weeper")
		{
			auto weeper = dynamic_cast<Weeper*>(m);				//등장씬으로 쓸만한 애니메이션이 생각이 안남
			EventHandler::GetInstance()->AddEvent("ANIM_TO_WEEPER_IDLE", 0.5f, weeper);
		}
	}

	m_deactivate = true;
	SetRemoveReserved();
	//클라이언트에게 컷씬 알림을 보내려면 여기에.
}

void TriggerObject2::Attribute_BossCutscene_1()
{
	if (m_attribute != TRIGGERATTRIBUTE::CUTSCENE6)
		return;

	auto monsterlist = ObjectManager::GetInstance()->GetLayer(L"Layer_Monster")->GetGameObjects();
	if (monsterlist.empty())
		return;

	for (auto& m : monsterlist)
	{
		if (m->GetName() == L"Golem")
		{
			auto golem = dynamic_cast<Golem*>(m);
			EventHandler::GetInstance()->AddEvent("ANIM_TO_GOLEM_ROAR", 4.f, golem);		//ROAR 애니메이션 재생 시간 2.33
			EventHandler::GetInstance()->AddEvent("ANIM_TO_GOLEM_IDLE", 6.32f, golem);		//ROAR > IDLE로 애니메이션 진행
		}
	}

	m_deactivate = true;
	SetRemoveReserved();
	//클라이언트에게 컷씬 알림을 보내려면 여기에.
}

void TriggerObject2::SendPlayers()
{
	//HERE
	auto objmgr = ObjectManager::GetInstance();
	auto players = objmgr->GetLayer(L"Layer_Player")->GetGameObjects();

	float givenSpace = 500.f;         //플레이어들 사이 공간
	int num = -1;

	for (auto& p : players)
	{
		auto player = dynamic_cast<Player*>(p);
		auto requestedPos = m_portalDestination[static_cast<int>(m_attribute)];

		if (m_attribute == TRIGGERATTRIBUTE::PORTAL4)
			requestedPos.z += givenSpace * num++;
		else
			requestedPos.x += givenSpace * num++;

		player->SetControllerPosition(requestedPos);
	}

	// 서버가 클라이언트에게 너희들을 이동시켰다고 알려줘야한다.
	// 이 코드가 플레이어를 이동시키므로 그동안 렌더링 로드/삭제 + 페이드 인, 아웃하면 된다.

	for (int32_t i = 0; i < SEND_AGAIN; ++i)
	{
		ServerSendPortalOutMessage();
	}
}

std::vector<Player*> TriggerObject2::OverlapCheck_Player()
{
	auto physDevice = PhysDevice::GetInstance();
	auto query = physDevice->GetQuery();

	std::vector<Player*> validPtrs;

	if (!m_box)
		return validPtrs;									//여기 걸리면 SkillAdd()의 오류. 각 Monster의 Init수정 요구

	physx::PxTransform trans;								//위치정보는 mapobj와 동일
	trans = m_body->GetGlobalPose();

	PxQueryFilterData filterData(PxQueryFlag::eDYNAMIC | PxQueryFlag::eSTATIC);		//overlap진행
	constexpr PxU32 bufferSize = 10;  // Set this to whatever size you need
	PxOverlapHit hitBuffer[bufferSize];
	PxOverlapBuffer overlapBuffer(hitBuffer, bufferSize);

	query->Overlap(*m_box, trans, filterData, &overlapBuffer);

	for (PxU32 i = 0; i < overlapBuffer.getNbTouches(); ++i)
	{
		const PxOverlapHit& hit = overlapBuffer.getTouch(i);
		PxShape* shape = hit.shape;
		PxRigidActor* actor = shape->getActor();

		auto body = static_cast<RigidBody*>(actor->userData);

		GameObject* obj = body->GetOwnerObject();
		if (!obj)
			continue;											// 1 : ownerobj 유효포인터 체크
		std::string name = body->GetName();
		if (name == "Player")
		{
			auto player = dynamic_cast<Player*>(body->GetOwnerObject());
			validPtrs.emplace_back(player);
			int size = validPtrs.size();
		}
	}

	return validPtrs;
}

void TriggerObject2::Handle_OverlapOut(const std::vector<Player*>& validptr)
{
	if (m_oneTimeOnly)
		return;

	for (auto it = m_duplicates.begin(); it != m_duplicates.end(); )
	{
		bool foundInValidPtr = std::find(validptr.begin(), validptr.end(), *it) != validptr.end();
		if (foundInValidPtr)  // validptr와 duplicates의 중복은 패스.
		{
			++it;
		}
		else  // If not found in validptr, then erase from m_vector.
		{
			if (!m_oneTimeOnly)
			{
				ApplyOut(*it);
				it = m_duplicates.erase(it);
			}
		}
	}
}

bool TriggerObject2::IsPlayerDuplicate(GameObject* obj)
{
	if (std::find(m_duplicates.begin(), m_duplicates.end(), obj) == m_duplicates.end())
		return false;

	return true;
}

bool TriggerObject2::Apply(Player* player)
{
	if (m_oneTimeOnly)
	{
		//cout << "attribute once" << endl;

		for (int32_t i = 0; i < SEND_AGAIN; ++i)
		{
			ServerSendInMessage(player);
		}

		return true;
	}
	else
	{
		//cout << "attribute loop" << endl;

		for (int32_t i = 0; i < SEND_AGAIN; ++i)
		{
			ServerSendInMessage(player);
		}

		return false;
	}
}

void TriggerObject2::ApplyOut(Player* player)
{
	switch (m_attribute)
	{
		case TRIGGERATTRIBUTE::GUIDELINE1:
		{
			//loop out
		}
		break;
	}
}

void TriggerObject2::ServerInit()
{
	//서버 생성 명령		(포탈/UI알림이가 목적이므로 웬만하면 쓸일 없을것이다.)
}

void TriggerObject2::ServerRelease()
{
	//서버 삭제 명령		(포탈/UI알림이가 목적이므로 웬만하면 쓸일 없을것이다.)
}

void TriggerObject2::ServerSendInMessage(Player* player)
{
	switch (m_attribute)
	{
		case TRIGGERATTRIBUTE::GUIDELINE1:
		{
			ServerSendTriggerInMessage(player, server::TRIGGER_INTERACTION_TYPE::GUIDE_UI1);
		}
		break;
		case TRIGGERATTRIBUTE::CUTSCENE1:
		{
			ServerSendCutSceneMessage(server::CUT_SCENE_TYPE::SCENE1);
		}
		break;
		case TRIGGERATTRIBUTE::CUTSCENE2:
		{
			m_deactivate = true;
			SetRemoveReserved();
			ServerSendCutSceneMessage(server::CUT_SCENE_TYPE::SCENE2);
		}
		break;
		case TRIGGERATTRIBUTE::CUTSCENE3:
		{
			ServerSendCutSceneMessage(server::CUT_SCENE_TYPE::SCENE3);
		}
		break;
		case TRIGGERATTRIBUTE::CUTSCENE4:
		{
			ServerSendCutSceneMessage(server::CUT_SCENE_TYPE::SCENE4);
		}
		break;
		case TRIGGERATTRIBUTE::CUTSCENE5:
		{
			ServerSendCutSceneMessage(server::CUT_SCENE_TYPE::SCENE5);
		}
		break;
		case TRIGGERATTRIBUTE::CUTSCENE6:
		{
			ServerSendCutSceneMessage(server::CUT_SCENE_TYPE::SCENE6);
		}
		break;
		case TRIGGERATTRIBUTE::PORTAL1:
		{
			ServerSendTriggerInMessage(player, server::TRIGGER_INTERACTION_TYPE::PORTAL1_IN);
		}
		break;
		case TRIGGERATTRIBUTE::PORTAL2:
		{
			ServerSendTriggerInMessage(player, server::TRIGGER_INTERACTION_TYPE::PORTAL2_IN);
		}
		break;
		case TRIGGERATTRIBUTE::PORTAL3:
		{
			ServerSendTriggerInMessage(player, server::TRIGGER_INTERACTION_TYPE::PORTAL3_IN);
		}
		break;
		case TRIGGERATTRIBUTE::PORTAL4:
		{
			ServerSendTriggerInMessage(player, server::TRIGGER_INTERACTION_TYPE::PORTAL4_IN);
		}
		break;
		case TRIGGERATTRIBUTE::PORTAL5:
		{
			ServerSendTriggerInMessage(player, server::TRIGGER_INTERACTION_TYPE::PORTAL5_IN);
		}
		break;
		default:
		break;
	}
}

void TriggerObject2::ServerSendTriggerInMessage(Player* player, server::TRIGGER_INTERACTION_TYPE triggerType)
{
	game::TimerEvent ev{ ProtocolID::WR_TRIGGER_INTERACTION_ACK };
	ev.objID = player->GetID();
	ev.integer = magic_enum::enum_integer(triggerType);

	MSG_HANDLER->PushSendMessage(ev);
}

void TriggerObject2::ServerSendPortalOutMessage()
{
	//조건을 확인하고 이에 따라 서버 메시지를 전송한다.
	//Detect함수에서 조건 확인 후 호출

	game::TimerEvent ev{ ProtocolID::WR_TRIGGER_INTERACTION_ACK };
	ev.objID = m_id;

	if (m_attribute == TRIGGERATTRIBUTE::PORTAL1)
		ev.integer = magic_enum::enum_integer(server::TRIGGER_INTERACTION_TYPE::PORTAL1_OUT);
	else if (m_attribute == TRIGGERATTRIBUTE::PORTAL2)
		ev.integer = magic_enum::enum_integer(server::TRIGGER_INTERACTION_TYPE::PORTAL2_OUT);
	else if (m_attribute == TRIGGERATTRIBUTE::PORTAL3)
		ev.integer = magic_enum::enum_integer(server::TRIGGER_INTERACTION_TYPE::PORTAL3_OUT);
	else if (m_attribute == TRIGGERATTRIBUTE::PORTAL4)
		ev.integer = magic_enum::enum_integer(server::TRIGGER_INTERACTION_TYPE::PORTAL4_OUT);
	else if (m_attribute == TRIGGERATTRIBUTE::PORTAL5)
		ev.integer = magic_enum::enum_integer(server::TRIGGER_INTERACTION_TYPE::PORTAL5_OUT);

	MSG_HANDLER->PushSendMessage(ev);
}

void TriggerObject2::ServerSendInteractionCountMessage()
{
	game::TimerEvent ev{ ProtocolID::WR_TRIGGER_INTERACTION_COUNT_ACK };
	ev.objID = m_id;
	ev.integer = m_currentCnt;

	if (m_attribute == TRIGGERATTRIBUTE::PORTAL1)
		ev.state = magic_enum::enum_integer(server::TRIGGER_INTERACTION_TYPE::PORTAL1_IN);
	else if (m_attribute == TRIGGERATTRIBUTE::PORTAL2)
		ev.state = magic_enum::enum_integer(server::TRIGGER_INTERACTION_TYPE::PORTAL2_IN);
	else if (m_attribute == TRIGGERATTRIBUTE::PORTAL3)
		ev.state = magic_enum::enum_integer(server::TRIGGER_INTERACTION_TYPE::PORTAL3_IN);
	else if (m_attribute == TRIGGERATTRIBUTE::PORTAL4)
		ev.state = magic_enum::enum_integer(server::TRIGGER_INTERACTION_TYPE::PORTAL4_IN);
	else if (m_attribute == TRIGGERATTRIBUTE::PORTAL5)
		ev.state = magic_enum::enum_integer(server::TRIGGER_INTERACTION_TYPE::PORTAL5_IN);

	MSG_HANDLER->PushSendMessage(ev);
}

void TriggerObject2::ServerSendCutSceneMessage(server::CUT_SCENE_TYPE cutSceneType)
{
	game::TimerEvent ev{ ProtocolID::WR_PLAY_CUT_SCENE_ACK };
	ev.objID = m_id;
	ev.integer = magic_enum::enum_integer(cutSceneType);

	MSG_HANDLER->PushSendMessage(ev);
}

bool TriggerObject2::CheckArtifactDestoryed()
{
	Layer* artiLayer = ObjectManager::GetInstance()->GetLayer(L"Layer_Gimmik_Artifact");

	if (artiLayer == nullptr)
		return false;											// 레이어 유무 확인

	if (artiLayer->GetGameObjects().size() == 0)				// 리스트 크기 확인. 0일 경우 true
	{
		m_deactivate = true;
		SetRemoveReserved();

		return true;
	}

	return false;												// 리스트 크기가 0이 아니면 false
}

void TriggerObject2::AttributeGimmik2()
{
	//GIMMIK2 값을 가진 TriggerObject2의 위치 16217, -1040, 29064. 해당 객체의(박스)의 halfExtent는 1500정도면 적당하다. 직접 실험해보고 박스 크기를 변경
	// 1. GIMMIK2값을 가진 박스를 설치
	// 2. 설치된 박스로 ThrowGimmik이 문제없이 실행되는지 확인 (공이 제대로 1개만 던져지는가, 기존과 같이 공이 적절하게 벽을 파괴하는가, 부서진 벽에서 엉뚱한 물리현상이 일어나 벽돌이 부자연스럽게 튀어오르지는 않는가 등)
	//																																이건 pvd에서 실행해보고 여러번 돌려보며 확인
	//																																정확히 같은 위치에서 같은 힘으로 던지기 때문에 아마 같은 결과가 나오겠지만 꼭 재확인해야한다.
	// 3. 컷씬 설치

	//서버 클라간 필요한 코드 작성.

	//컷씬 실행에 시간이 필요하다면 돌을 던지는 시간을 조정할 수 있다. 중간값이 5.f면 5초후 돌을 던진다.
	EventHandler::GetInstance()->AddEvent("THROWGIMMIK2BALL", 0.1f, this);

	m_deactivate = true;
	//SetRemoveReserved();
}

void TriggerObject2::ThrowGimmik2Ball()
{
	auto objmgr = ObjectManager::GetInstance();

	float boulderPower = 100000.f;
	auto boulderObj = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Gimmik_Boulder", Vec3(16276.514, -140, 26758.246), Quat(0, 0, 0, 1), Vec3(300, 300, 300));
	auto boulderBody = boulderObj->GetComponent<RigidBody>(L"RigidBody");

	boulderBody->AddCollider<SphereCollider>(boulderObj->GetTransform()->GetScale());
	boulderObj->ApplyRequestedLayers();
	boulderBody->SetKinematic(false);
	boulderBody->SetRigidBodySleep(false);
	boulderBody->SetMass(500.f);

	auto boulderCollider = boulderBody->GetCollider(0);
	boulderCollider->SetRestitution(0.8f);
	boulderCollider->SetRestitutionCombineMode(PhysicsCombineMode::Max);
	boulderCollider->SetFriction(0.4f);
	boulderCollider->SetFrictionCombineMode(PhysicsCombineMode::Average);

	physx::PxVec3 dir = physx::PxVec3(16215, -2558, 35219) - boulderBody->GetPosition();
	dir.normalize();

	boulderBody->AddForce(ForceMode::Impulse, dir * boulderPower);

	boulderObj->ServerMessage_Init(false, true);      //0723
}
