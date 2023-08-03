#include "pch.h"
#include "TriggerObject2.h"
#include "PhysDevice.h"
#include "physx_utils.h"
#include "RigidBody.h"
#include "Collider.h"
#include "Player.h"
#include "Transform.h"

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
}

void TriggerObject2::Update(double timeDelta)
{
	MapObject::Update(timeDelta);				//removeReserved 체크

	Handle_Overlap();
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

void TriggerObject2::ServerSendMessage()
{
	//조건을 확인하고 이에 따라 서버 메시지를 전송한다.
	//Detect함수에서 조건 확인 후 호출
}
