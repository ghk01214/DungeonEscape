#include "pch.h"
#include "Monster.h"
#include "Dragon.h"
#include "CustomController.h"
#include "MessageHandler.h"
#include "TimeManager.h"
#include "RigidBody.h"
#include "Transform.h"
#include "Collider.h"
#include "CollisionPairInfo.h"
#include "Player.h"
#include "RigidBody.h"

Dragon::Dragon(int32_t MonsterID, const Vec3& position, const Quat& rotation, const Vec3& scale) :
	Monster{ MonsterID, position, rotation, scale },
	m_prevState{ IDLE1 },
	m_currState{ m_prevState }
{
	m_hp = 20;

	m_name = L"Dragon";
	m_objType = server::OBJECT_TYPE::BOSS;
	m_fbxType = server::FBX_TYPE::DRAGON;
}

Dragon::~Dragon()
{
}

void Dragon::Init()
{
	Monster::Init();
}

void Dragon::Update(double timeDelta)
{
	if (IsDead() == true)
	{
		if (m_currState != DEAD)
			m_currState = DIE;

		Monster::Update(timeDelta);

		return;
	}

	// 5초마다 드래곤 패턴 발동
	using namespace std::chrono;
	static steady_clock::time_point bossSkillEndTime{ CURRENT_TIME };

	if (CURRENT_TIME - bossSkillEndTime > 5s)
	{
		DragonPattern_GroundHit();
		bossSkillEndTime = CURRENT_TIME;
	}
	//DragonPattern_GroundHit();

	Monster::Update(timeDelta);
}

void Dragon::LateUpdate(double timeDelta)
{
	CheckState();
	UpdateFrame();

	Monster::LateUpdate(timeDelta);
}

void Dragon::Release()
{
	Monster::Release();
}

void Dragon::CheckState()
{
	if (m_prevState == m_currState)
		return;

	switch (m_currState)
	{
		case ATTACK_HAND:
		{

		}
		break;
		case ATTACK_HORN:
		{

		}
		break;
		case ATTACK_MOUTH:
		{

		}
		break;
		case DEFEND:
		{

		}
		break;
		case DIE:
		{
			std::cout << "BOSS DEFEATED!\n";
		}
		break;
		case DAMAGE:
		{
		}
		break;
		case DEAD:
		{
			SetRemoveReserved();

			for (int32_t i = 0; i < SEND_AGAIN - 1; ++i)
			{
				game::TIMER_EVENT ev{ ProtocolID::WR_REMOVE_ACK };
				ev.objID = m_id;
				ev.objType = m_objType;

				game::MessageHandler::GetInstance()->PushSendMessage(ev);
			}
		}
		break;
		case IDLE1:
		{

		}
		break;
		case IDLE2:
		{

		}
		break;
		case JUMP:
		{

		}
		break;
		case RUN:
		{

		}
		break;
		case SCREAM:
		{

		}
		break;
		case SLEEP:
		{

		}
		break;
		case WALK:
		{

		}
		break;
		case WALK_BACK:
		{

		}
		break;
		case WALK_LEFT:
		{

		}
		break;
		case WALK_RIGHT:
		{

		}
		break;
		default:
		break;
	}

	m_prevState = m_currState;

	game::TIMER_EVENT ev{ ProtocolID::WR_CHANGE_STATE_ACK };
	ev.state = m_currState;
	ev.objID = m_id;
	ev.objType = m_objType;

	game::MessageHandler::GetInstance()->PushSendMessage(ev);

	//std::cout << "DRAGON STATE : " << magic_enum::enum_name(m_currState) << "\n";
}

void Dragon::UpdateFrame()
{
	switch (m_currState)
	{
		case IDLE1: case IDLE2: case RUN: case DEAD:
		case WALK: case WALK_BACK: case WALK_LEFT: case WALK_RIGHT:
		return;
		default:
		break;
	}

	if (m_aniEnd == false)
		return;

	switch (m_currState)
	{
		case ATTACK_HAND: case ATTACK_HORN: case ATTACK_MOUTH:
		{
			m_currState = IDLE1;
		}
		break;
		case DEFEND:
		{
			m_currState = IDLE1;
		}
		break;
		case DIE:
		{
			m_currState = DEAD;
			m_startSendTransform = false;
		}
		break;
		case DAMAGE: case JUMP: case SCREAM: case SLEEP:
		{
			// 추후 각 state 별 추가 사항이 있으면 case 분리
			m_currState = IDLE1;
		}
		break;
		default:
		break;

	}

	m_aniEnd = false;
}

void Dragon::DragonPattern_GroundHit()
{
	Collider* surfaceCollider = m_controller->GetColliderBelow();
	if (surfaceCollider == nullptr)
		return;

	physx::PxVec3 monsterPos = m_controller->GetBody()->GetPosition();

	auto collisionEnterLists = surfaceCollider->GetCollisionInfo(CollisionInfoType::Enter);
	auto collisionStayLists = surfaceCollider->GetCollisionInfo(CollisionInfoType::Stay);

	std::set<Collider*> enTouch;		//몬스터와 동일한 메쉬에 서있는 콜라이더들

	for (auto& info : collisionStayLists)
	{
		auto collider = info.get()->GetFromCollider();
		auto obj = collider->GetOwnerObject();
		auto result = dynamic_cast<Player*>(obj);

		if (result == nullptr or result->IsDead() == true)
			continue;

		auto playerController = result->GetController();
		auto playerbody = playerController->GetBody();
		physx::PxVec3 playerPos = playerbody->GetPosition();
		physx::PxVec3 direction = playerPos - monsterPos;
		float distance = direction.magnitude();

		direction.normalize();

		float verticalStrength = 1.f;
		float horizontalStrength = 1.f;
		if (distance < 300)
		{
			horizontalStrength = 300.f;
			verticalStrength = 1000.f;
		}
		else
		{
			horizontalStrength = 100.f;
			verticalStrength = 600.f;
		}

		playerController->BounceFromAttack();
		playerbody->AddForce(ForceMode::Impulse, physx::PxVec3(direction.x * horizontalStrength, verticalStrength, direction.z * horizontalStrength));

		std::cout << "몬스터 패턴 발동, 플레이어 피격 적용\n";

		result->SetState(Player::PLAYER_STATE::DAMAGE);
		//result->GetDamaged(5);
		std::cout << "PLAYER [" << result->GetID() << "] HP : " << result->GetHP() << "\n";

		if (result->IsDead() == true)
		{
			std::cout << "PLAYER [" << result->GetID() << "] DEAD!\n";

			result->SetState(Player::PLAYER_STATE::DIE0);
		}

		//playerbody->SetVelocity(physx::PxVec3(direction.x * 100, 100, direction.z * 100));
	}
}

Dragon::DRAGON_STATE Dragon::GetState() const
{
	return m_currState;
}

void Dragon::SetState(DRAGON_STATE state)
{
	m_currState = state;
}
