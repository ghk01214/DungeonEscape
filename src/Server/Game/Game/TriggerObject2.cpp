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

	GeometryInit(m_transform->GetScale());

	ServerInit();
	SetPortalDestination();
}

void TriggerObject2::Update(double timeDelta)
{
	MapObject::Update(timeDelta);				//removeReserved 체크

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
			//bool applied = Apply(player);							//중복아니면 명령 후 중복벡터에 등록
			//if (applied)
			//	m_duplicates.emplace_back(player);
			Apply(player);
			m_duplicates.emplace_back(player);

		}
	}


	// HERE : 인원수에 변화가 생길 경우 출력.
	m_currentCnt = m_duplicates.size();
	if (m_currentCnt != m_beforeCnt)
	{
		m_beforeCnt = m_currentCnt;
		cout << m_currentCnt << endl;
	}
}

void TriggerObject2::SetPortalDestination()
{
	//HERE
	m_portalDestination.resize(static_cast<int>(TRIGGERATTRIBUTE::END));

	m_portalDestination[static_cast<int>(TRIGGERATTRIBUTE::PORTAL1)] = Vec3(0.f, -550.f, 8020.f);
	m_portalDestination[static_cast<int>(TRIGGERATTRIBUTE::PORTAL2)] = Vec3(0.f, -550.f, 13520.f);
	m_portalDestination[static_cast<int>(TRIGGERATTRIBUTE::PORTAL3)] = Vec3(0.f, -1590.f, 19420.f);
	m_portalDestination[static_cast<int>(TRIGGERATTRIBUTE::PORTAL4)] = Vec3(15000.f, -1590.f, 24570.f);
	m_portalDestination[static_cast<int>(TRIGGERATTRIBUTE::PORTAL5)] = Vec3(16216.f, -3690.f, 40993.f);

	//여기서 포탈 1, 포탈2, 등의 이동 위치를 설정한다.
}

void TriggerObject2::AttributePortal(double timeDelta)
{
	string name = string(magic_enum::enum_name(m_attribute));

	if (m_duplicates.empty())
	{
		EventHandler::GetInstance()->DeleteEvent(name);
		return;
	}
	else
	{
		EventHandler::GetInstance()->AddEventIfNone(name, m_requestedContactTime, this);
		return;
	}


	//EventHandler::GetInstance()->AddEventIfNone("TRIGGERCLOCK")
}

void TriggerObject2::SendPlayers()
{
	//HERE
	auto objmgr = ObjectManager::GetInstance();
	auto players = objmgr->GetLayer(L"Layer_Player")->GetGameObjects();

	for (auto& p : players)
	{
		auto player = dynamic_cast<Player*>(p);
		player->SetControllerPosition(m_portalDestination[static_cast<int>(m_attribute)]);
	}

	// 서버가 클라이언트에게 너희들을 이동시켰다고 알려줘야한다.
	// 이 코드가 플레이어를 이동시키므로 그동안 렌더링 로드/삭제 + 페이드 인, 아웃하면 된다.
	ServerSendPortalOutMessage();
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
				it = m_duplicates.erase(it);
				cout << "loop out" << endl;
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
		cout << "attribute once" << endl;
		return true;
	}
	else
	{
		cout << "attribute loop" << endl;

		ServerSendInMessage();

		return false;
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

void TriggerObject2::ServerSendInMessage()
{
	switch (m_attribute)
	{
		case TRIGGERATTRIBUTE::PORTAL1:
		case TRIGGERATTRIBUTE::PORTAL2:
		case TRIGGERATTRIBUTE::PORTAL3:
		case TRIGGERATTRIBUTE::PORTAL4:
		case TRIGGERATTRIBUTE::PORTAL5:
		case TRIGGERATTRIBUTE::PORTAL6:
		case TRIGGERATTRIBUTE::PORTAL7:
		case TRIGGERATTRIBUTE::PORTAL8:
		{
			ServerSendPortalInMessage();
		}
		break;
		case TRIGGERATTRIBUTE::GUIDELINE1:
		case TRIGGERATTRIBUTE::GUIDELINE2:
		case TRIGGERATTRIBUTE::GUIDELINE3:
		case TRIGGERATTRIBUTE::GUIDELINE4:
		case TRIGGERATTRIBUTE::GUIDELINE5:
		{
		}
		break;
		default:
		break;
	}
}

void TriggerObject2::ServerSendPortalInMessage()
{
	game::TIMER_EVENT ev{ ProtocolID::WR_TRIGGER_INTERACTION_ACK };

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
	else if (m_attribute == TRIGGERATTRIBUTE::PORTAL6)
		ev.state = magic_enum::enum_integer(server::TRIGGER_INTERACTION_TYPE::PORTAL6_IN);

	ev.objID = m_id;

	game::MessageHandler::GetInstance()->PushSendMessage(ev);
}

void TriggerObject2::ServerSendPortalOutMessage()
{
	//조건을 확인하고 이에 따라 서버 메시지를 전송한다.
	//Detect함수에서 조건 확인 후 호출

	game::TIMER_EVENT ev{ ProtocolID::WR_TRIGGER_INTERACTION_ACK };

	if (m_attribute == TRIGGERATTRIBUTE::PORTAL1)
		ev.state = magic_enum::enum_integer(server::TRIGGER_INTERACTION_TYPE::PORTAL1_OUT);
	else if (m_attribute == TRIGGERATTRIBUTE::PORTAL2)
		ev.state = magic_enum::enum_integer(server::TRIGGER_INTERACTION_TYPE::PORTAL2_OUT);
	else if (m_attribute == TRIGGERATTRIBUTE::PORTAL3)
		ev.state = magic_enum::enum_integer(server::TRIGGER_INTERACTION_TYPE::PORTAL3_OUT);
	else if (m_attribute == TRIGGERATTRIBUTE::PORTAL4)
		ev.state = magic_enum::enum_integer(server::TRIGGER_INTERACTION_TYPE::PORTAL4_OUT);
	else if (m_attribute == TRIGGERATTRIBUTE::PORTAL5)
		ev.state = magic_enum::enum_integer(server::TRIGGER_INTERACTION_TYPE::PORTAL5_OUT);
	else if (m_attribute == TRIGGERATTRIBUTE::PORTAL6)
		ev.state = magic_enum::enum_integer(server::TRIGGER_INTERACTION_TYPE::PORTAL6_OUT);

	ev.objID = m_id;

	game::MessageHandler::GetInstance()->PushSendMessage(ev);
}
