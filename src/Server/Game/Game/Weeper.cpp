#include "pch.h"
#include "Weeper.h"
#include "WeeperAI.h"
#include "SkillObject.h"
#include "CustomController.h"
#include "MessageHandler.h"
#include "TimeManager.h"
#include "RigidBody.h"
#include "Transform.h"
#include "Collider.h"
#include "CollisionPairInfo.h"
#include "CapsuleCollider.h"
#include "ObjectManager.h"
#include "Layer.h"
#include "Player.h"
#include "RigidBody.h"
#include "MonsterAI.h"
#include "EventHandler.h"

using namespace std;

Weeper::Weeper(int32_t MonsterID, const Vec3& position, const Quat& rotation, const Vec3& scale) :
	Monster{ MonsterID, position, rotation, scale },
	m_prevState{ IDLE },
	m_currState{ m_prevState },
	m_sendState{ 0 }
{
	m_hp = 1000;
	m_maxHp = 1000;

	m_name = L"Weeper";
	m_objType = server::OBJECT_TYPE::BOSS;
	m_fbxType = server::FBX_TYPE::WEEPER1;
}

Weeper::~Weeper()
{
}

void Weeper::Init()
{
	Monster::Init();
	m_hp = 1000;

	m_controller->GetBody()->SetMass(1000.f);

	m_weeperAI = new WeeperAI(this);
	m_weeperAI->Init();
	m_AI = m_weeperAI;
}

void Weeper::Update(double timeDelta)
{
	SendChangedStateAgain();

	m_weeperAI->Update(timeDelta);

	CheckState();
	UpdateFrameOnce();

	Monster::Update(timeDelta);
}

void Weeper::LateUpdate(double timeDelta)
{
	Monster::LateUpdate(timeDelta);
}

void Weeper::Release()
{
	Monster::Release();

	SafeRelease(m_weeperAI);
}

void Weeper::CheckState()
{
	if (m_prevState == m_currState)
		return;

	switch (m_currState)
	{
		case CAST1:
		{

		}
		break;
		case CAST2_START:
		{

		}
		break;
		case CAST2_LOOP:
		{

		}
		break;
		case CAST2_END:
		{
		}
		break;
		case CAST3:
		{

		}
		break;
		case CAST4_START:
		{

		}
		break;
		case CAST4_LOOP:
		{

		}
		break;
		case CAST4_END:
		{
		}
		break;
		case DAMAGE:
		{
		}
		break;
		case DEAD:
		{
		}
		break;
		case DEATH:
		{
		}
		break;
		case DODGE:
		{

		}
		break;
		case IDLE:
		{

		}
		break;
		case IDLE_BREAK:
		{

		}
		break;
		case STATUE1:
		{

		}
		break;
		case STATUE2:
		{

		}
		break;
		case STATUE3:
		{

		}
		break;
		case TAUNT:
		{

		}
		break;
		case TURN_LEFT:
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
		case WALK_BACK_NO_LEGS:
		{

		}
		break;
		default:
		break;
	}

	m_prevState = m_currState;
	m_sendState = SEND_AGAIN;

	//for (int32_t i = 0; i < SEND_AGAIN; ++i)
	{
		//cout << "호출되긴한거냐?" << endl;
		game::TIMER_EVENT ev{ ProtocolID::WR_CHANGE_STATE_ACK };
		ev.state = m_currState;
		ev.objID = m_id;
		ev.objType = m_objType;

		game::MessageHandler::GetInstance()->PushSendMessage(ev);
	}

}

void Weeper::UpdateFrameOnce()
{
	switch (m_currState)
	{
		case CAST2_LOOP: case CAST4_LOOP: case IDLE: case DEAD:
		case STATUE1: case STATUE2: case STATUE3:
		case WALK: case WALK_BACK: case WALK_BACK_NO_LEGS:
		return;
		default:
		break;
	}

	if (m_aniEnd == false)
		return;

	switch (m_currState)
	{
		case CAST1: case CAST3: case CAST4_END:
		{
			m_currState = IDLE;
			m_weeperAI->UpdateTargetPos();		//패턴이 끝났으면 바로 플레이어를 바라보도록
		}
		break;
		case CAST2_END:
		{
			m_currState = IDLE;
		}
		break;
		case CAST2_START: case CAST4_START:
		{
		}
		break;
		case DEATH:
		{
			m_currState = DEAD;
			m_startSendTransform = false;
		}
		break;
		case DAMAGE: case DODGE: case IDLE_BREAK:
		case TAUNT: case TURN_LEFT: case TURN_RIGHT:
		{
			// 추후 각 state 별 추가 사항이 있으면 case 분리
			m_currState = IDLE;
		}
		break;
		default:
		break;
	}

	m_aniEnd = false;
}

WeeperAI* Weeper::GetAI()
{
	return m_weeperAI;
}

void Weeper::Pattern_Cast1()
{
#pragma region 순서 설정
	static int order = -1;
	order += 1;
	if (order > 2)
		order = 0;
#pragma endregion

	//투사체 위치 선정
	physx::PxVec3 ballPos = m_controller->GetBody()->GetGlobalPose().p;
	ballPos.y += 550.f;		//weeper 모델 위치 고려해서 살짝 위로

	float xPosInterval = 400.f;
	static float xTurn = Randnum_Cast1_XInterval();

	physx::PxVec3 lookDir = TO_PX3(m_weeperAI->m_targetDir);
	physx::PxVec3 up(0, 1, 0);
	physx::PxVec3 right = lookDir.cross(up);
	right.getNormalized();

	float monsterRadius = m_controller->GetCollider()->GetRadius();
	float skillBallHalfExtent = 100.f;

	switch (order)
	{
		case 0:
		break;

		case 1:
		ballPos += right * xPosInterval * xTurn;
		ballPos.y += Randnum_Cast1_YInterval();
		break;

		case 2:
		ballPos -= right * xPosInterval * xTurn;
		ballPos.y += Randnum_Cast1_YInterval();			//상하 간격으로 상시 랜덤으로 돌린다
		xTurn = Randnum_Cast1_XInterval();				//3번째공을 소환하면 좌우 생성 순서 다시 랜덤으로 설정
		break;
	}


	Vec3 convertedballPos = FROM_PX3(ballPos);

	SkillObject::SKILLOBJECTTYPE skilltype = SkillObject::SKILLOBJECTTYPE::WEEPER_CAST1_BALL;

	//투사체 생성
	auto objmgr = ObjectManager::GetInstance();
	auto layer = objmgr->GetLayer(L"Layer_SkillObject");
	auto skillObject = objmgr->AddGameObjectToLayer<SkillObject>
		(L"Layer_SkillObject", convertedballPos, Quat(0, 0, 0, 1), Vec3(skillBallHalfExtent, skillBallHalfExtent, skillBallHalfExtent), skilltype, m_weeperAI->m_target, this);
}

SkillObject* Weeper::Pattern_Cast2()
{
	physx::PxVec3 ballPos = m_controller->GetBody()->GetGlobalPose().p;
	ballPos.y += 800.f;		//weeper 모델 위치 고려해서 살짝 위로

	float monsterRadius = m_controller->GetCollider()->GetRadius();
	float skillBallHalfExtent = 200.f;

	Vec3 convertedballPos = FROM_PX3(ballPos);

	SkillObject::SKILLOBJECTTYPE skilltype = SkillObject::SKILLOBJECTTYPE::WEEPER_CAST2_BALL;

	//투사체 생성
	auto objmgr = ObjectManager::GetInstance();
	auto layer = objmgr->GetLayer(L"Layer_SkillObject");
	SkillObject* skillObject = objmgr->AddGameObjectToLayer<SkillObject>
		(L"Layer_SkillObject", convertedballPos, Quat(0, 0, 0, 1), Vec3(skillBallHalfExtent, skillBallHalfExtent, skillBallHalfExtent), skilltype, m_weeperAI->m_target, this);

	return skillObject;
}

void Weeper::Pattern_Cast2_Scatter()
{
	physx::PxVec3 ballPos = m_controller->GetBody()->GetGlobalPose().p;
	ballPos += Randnum_Cast2_XZInterval();

	float monsterRadius = m_controller->GetCollider()->GetRadius();
	float skillBallHalfExtent = 70.f;

	Vec3 convertedballPos = FROM_PX3(ballPos);

	SkillObject::SKILLOBJECTTYPE skilltype = SkillObject::SKILLOBJECTTYPE::WEEPER_CAST2_BALL_SCATTER;

	//투사체 생성
	auto objmgr = ObjectManager::GetInstance();
	auto layer = objmgr->GetLayer(L"Layer_SkillObject");
	auto skillObject = objmgr->AddGameObjectToLayer<SkillObject>
		(L"Layer_SkillObject", convertedballPos, Quat(0, 0, 0, 1), Vec3(skillBallHalfExtent, skillBallHalfExtent, skillBallHalfExtent), skilltype, m_weeperAI->m_target, this);
}

void Weeper::Pattern_Cast2_CounterNuclear()
{
	m_weeperAI->Cast2Counter_OFF();			//카운터 모드 종료
	SetState(WEEPER_STATE::CAST2_END);
	EventHandler::GetInstance()->AddEvent("CAST2_NUCLEAR_AIRFIRE", 0.5f, this);
}

SkillObject* Weeper::Pattern_Cast3()
{
	Vec3 ballPos= m_weeperAI->m_target->GetControllerPosition();
	ballPos.y += 600.f;

	float skillBallHalfExtent = 100.f;
	SkillObject::SKILLOBJECTTYPE skilltype = SkillObject::SKILLOBJECTTYPE::WEEPER_CAST3_BALL;

	//투사체 생성
	auto objmgr = ObjectManager::GetInstance();
	auto layer = objmgr->GetLayer(L"Layer_SkillObject");
	auto skillObject = objmgr->AddGameObjectToLayer<SkillObject>
		(L"Layer_SkillObject", ballPos, Quat(0, 0, 0, 1), Vec3(skillBallHalfExtent, skillBallHalfExtent, skillBallHalfExtent), skilltype, m_weeperAI->m_target, this);

	return skillObject;
}

bool Weeper::Pattern_Cast4()
{
	if (m_cast4_vertVel.y < 10.f)
		m_cast4_vertVel *= 0.9985f;

	else if (m_cast4_vertVel.y < 200.f)
		m_cast4_vertVel *= 0.995f;

	else if (m_cast4_vertVel.y < 500.f)
		m_cast4_vertVel *= 0.96f;
	else
		m_cast4_vertVel *= 0.92f;

	auto targetPlayer = m_weeperAI->m_target;
	auto targetController = targetPlayer->GetController();
	auto targetBody = targetPlayer->GetController()->GetBody();

	targetController->Keyboard_All_Clear();							//해당 마법에 당할때는 키보드 무력화

	if (m_cast4_vertVel.y > 0)
		targetBody->SetVelocity(m_cast4_vertVel);

	else
		targetBody->AddForce(ForceMode::Impulse, m_cast4_vertVel);


	if (m_cast4_vertVel.y < 1.f)
	{
		m_cast4_vertVel.y = -30.f;

		if (targetController->IsOnGround())
		{
			//타격
			m_cast4_vertVel = physx::PxVec3(0, 1, 0) * 800.f;
			Pattern_Cast4_Effect(targetPlayer);
			return true;
		}
	}

	return false;
}

void Weeper::Pattern_Cast4_Effect(Player* player)
{
	//적용된 힘은 pattern_cast4참고. 해당 함수가 호출된 if문에서 y값을 음수로 준다. 해당 값을 조절하면 내리꽂는 속도를 조절 가능하다.
	player->SetState(Player::PLAYER_STATE::DAMAGE);
}

void Weeper::Sink()
{
	m_controller->GetBody()->GetCollider(0)->ApplyModifiedLayer(PhysicsLayers::MONSTER, PhysicsLayers::MAP);
}

int Weeper::Randnum_Cast1_XInterval()
{
	static unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	static std::default_random_engine generator(seed);
	static std::uniform_int_distribution<int> distribution(0, 1);

	int number = distribution(generator) * 2 - 1;

	return number;
}

float Weeper::Randnum_Cast1_YInterval()
{
	static unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	static std::default_random_engine generator(seed);
	static std::uniform_int_distribution<int> distribution(-5, 5);

	int number = 0;
	while (number == 0) {
		// 랜덤 값을 생성
		number = distribution(generator) * 50;
	}

	return number;
}

physx::PxVec3 Weeper::Randnum_Cast2_XZInterval()
{
	static std::uniform_real_distribution<float> distribution(0, 2 * physx::PxPi); // 0 to 2*Pi for a full circle
	static std::uniform_real_distribution<float> distributionDistance(800.f, 1600.f);
	float startingY = 1000.f;

	float theta = distribution(dre); // get random angle
	float distance = distributionDistance(dre);

	// convert polar coordinates to cartesian
	float x = cos(theta) * distance;
	float z = sin(theta) * distance;

	physx::PxVec3 result(x, startingY, z);
	return result;
}

Weeper::WEEPER_STATE Weeper::GetState() const
{
	return m_currState;
}

void Weeper::SetState(WEEPER_STATE state)
{
	m_currState = state;
}

void Weeper::SendChangedStateAgain()
{
	if (m_sendState <= 0)
		return;

	game::TIMER_EVENT ev{ ProtocolID::WR_CHANGE_STATE_ACK, m_id };
	ev.state = m_currState;
	ev.objType = m_objType;

	game::MessageHandler::GetInstance()->PushSendMessage(ev);

	--m_sendState;
}
