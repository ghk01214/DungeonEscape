#include "pch.h"
#include "WeeperAI.h"
#include "Weeper.h"
#include "Monster.h"
#include "CustomController.h"
#include "SkillObject.h"
#include "EventHandler.h"

#include "MessageHandler.h"

using namespace std;

WeeperAI::WeeperAI(Weeper* weeper) :
	MonsterAI(weeper),
	m_weeper(weeper)
{
}

WeeperAI::~WeeperAI()
{
}

void WeeperAI::Init()
{
	MonsterAI::Init();

	//SkillSize 추가
	AddSkillSize("CAST1", GeometryType::Box, Vec3(50, 50, 600), false);		//z거리 1200
	AddSkillSize("CAST2", GeometryType::Box, Vec3(800, 800, 800), false);
	AddSkillSize("CAST3", GeometryType::Box, Vec3(50, 50, 600), false);
	AddSkillSize("CAST4", GeometryType::Box, Vec3(50, 50, 450), false);

	m_weeper->SetControllerMoveSpeed(10.f);

	//Schedule 추가
	//m_scheduler.emplace_back(WEEPER_SCHEDULE::CAST4);
}

void WeeperAI::Update(float timeDelta)
{
	MonsterAI::Update(timeDelta);

	DamageCheck();
}

void WeeperAI::LateUpdate(float timeDelta)
{
	MonsterAI::LateUpdate(timeDelta);
}

void WeeperAI::Release()
{
	MonsterAI::Release();
}

void WeeperAI::FillSchedule()
{
	if (!m_target)
		return;		// 초기 SetRandomTarget이 실패할 경우 탈출

	static std::uniform_int_distribution<int> count(1,3);
	static std::uniform_int_distribution<int> schedule(static_cast<int>(WEEPER_SCHEDULE::CAST1), static_cast<int>(WEEPER_SCHEDULE::CAST4));

	if (DeathHandle())
		return;

	m_scheduler.emplace_back(WEEPER_SCHEDULE::CAST1);
	m_scheduler.emplace_back(WEEPER_SCHEDULE::CAST2);
	m_scheduler.emplace_back(WEEPER_SCHEDULE::CAST3);
	m_scheduler.emplace_back(WEEPER_SCHEDULE::CAST4);



	std::cout << "Filled Schedule" << std::endl;
	ReportSchedule();
}

void WeeperAI::ExecuteSchedule(float deltaTime)
{
	if (IsEmptySchedule())
		return;

	if (m_weeper->m_currState != Weeper::IDLE && m_weeper->m_currState != Weeper::IDLE_BREAK)
		return;			// 기본스탠딩, 이동도중에만 패턴수행이 가능하다.

	static float directionUpdateInterval = 0.f;
	if (directionUpdateInterval > 1.0f)
	{
		UpdateTargetPos();
		directionUpdateInterval = 0.f;
	}
	directionUpdateInterval += deltaTime;

	WEEPER_SCHEDULE schedule = m_scheduler[0];

	bool inSkillRange = false;

	switch (schedule)
	{
		case WEEPER_SCHEDULE::CAST1:
		{
			inSkillRange = SkillRangeCheck();
			if (inSkillRange)
			{
				m_weeper->m_currState = Weeper::CAST1;
				m_weeper->SetMonsterImmobile();
				
				m_scheduler.erase(m_scheduler.begin());
				ReportSchedule();

				QuatUpdateForClient();

				EventHandler::GetInstance()->AddEvent("WEEPER_CAST1_FUNCTIONCALL", 1.5f, m_weeper);
				EventHandler::GetInstance()->AddEvent("WEEPER_CAST1_FUNCTIONCALL", 2.f, m_weeper);
				EventHandler::GetInstance()->AddEvent("WEEPER_CAST1_FUNCTIONCALL", 2.5f, m_weeper);

				SetAIWait(true);
				EventHandler::GetInstance()->AddEvent("ANIM_END", 6.36, m_weeper);
				EventHandler::GetInstance()->AddEvent("AI_WAIT_FREE", 7.f, m_weeper);

				ServerMessage_SendMonsterPattern(schedule);
			}
			else
			{
				Monstermove();
				m_weeper->m_currState = Weeper::IDLE;
			}
		}
		break;

		case WEEPER_SCHEDULE::CAST2:
		{
			inSkillRange = SkillRangeCheck();
			if (inSkillRange)
			{
				m_weeper->m_currState = Weeper::CAST2_START;										//원기옥 자세
				m_weeper->SetMonsterImmobile();

				SkillObject* cast2Ball = m_weeper->Pattern_Cast2();									//스킬오브젝트(CAST2_BALL) 생성
				Cast2Counter_ON();																	//원기옥 반격모드 ON
				m_AIWait = true;																	//fill,execute schedule 방지
				m_scheduler.erase(m_scheduler.begin());												//스케듈러 비우기

				EventHandler::GetInstance()->AddEvent("CAST2_SCATTER_AIRFIRE", 7.f, cast2Ball);		//EVENT:ASCEND예약 (스킬 ascend attribute 부여, 애니메이션 재생, 반격모드 OFF)

				ReportSchedule();

				QuatUpdateForClient();

				ServerMessage_SendMonsterPattern(schedule);
			}
			else
			{
				Monstermove();
				m_weeper->m_currState = Weeper::IDLE;
			}
		}
		break;

		case WEEPER_SCHEDULE::CAST3:
		{
			inSkillRange = SkillRangeCheck();
			if (inSkillRange)
			{
				m_weeper->m_currState = Weeper::CAST1;
				m_weeper->SetMonsterImmobile();

				m_scheduler.erase(m_scheduler.begin());
				ReportSchedule();

				QuatUpdateForClient();

				EventHandler::GetInstance()->AddEvent("WEEPER_CAST3_FUNCTIONCALL", 1.f, m_weeper);
				EventHandler::GetInstance()->AddEvent("WEEPER_CAST3_FUNCTIONCALL", 2.f, m_weeper);
				EventHandler::GetInstance()->AddEvent("WEEPER_CAST3_FUNCTIONCALL", 3.f, m_weeper);

				SetAIWait(true);
				EventHandler::GetInstance()->AddEvent("ANIM_END", 6.36f, m_weeper);
				EventHandler::GetInstance()->AddEvent("AI_WAIT_FREE", 7.f, m_weeper);

				ServerMessage_SendMonsterPattern(schedule);
			}
			else
			{
				Monstermove();
				m_weeper->m_currState = Weeper::IDLE;
			}
		}
		break;

		case WEEPER_SCHEDULE::CAST4:
		{
			inSkillRange = SkillRangeCheck();
			if (inSkillRange)
			{
				m_AIWait = true;
				m_weeper->SetState(Weeper::WEEPER_STATE::CAST4_START);
				m_weeper->SetMonsterImmobile();
				m_scheduler.erase(m_scheduler.begin());
				ReportSchedule();

				QuatUpdateForClient();

				EventHandler::GetInstance()->AddEvent("ANIM_TO_WEEPER_CAST4_LOOP", 2.73f, m_weeper);
				EventHandler::GetInstance()->AddEvent("WEEPER_CAST4", 2.73f, m_weeper);

				ServerMessage_SendMonsterPattern(schedule);
			}
			else
			{
				Monstermove();
				m_weeper->m_currState = Weeper::IDLE;
			}
		}
		break;
	}


}

bool WeeperAI::IsEmptySchedule()
{
	if (m_scheduler.empty())
		return true;

	return false;
}

std::string WeeperAI::GetRequestedScheduleName()
{
	if (m_scheduler.empty())
		return NULL;

	WEEPER_SCHEDULE schedule = m_scheduler[0];
	std::string name = magic_enum::enum_name(schedule).data();
	return name;
}

void WeeperAI::DamageCheck()
{
	static int originalHealth = m_weeper->GetHP();

	if (originalHealth > m_weeper->GetHP())
	{
		cout << "hp : " << originalHealth << endl;
		originalHealth = m_weeper->GetHP();
		if (m_cast2Counter)
		{
			m_weeper->Pattern_Cast2_CounterNuclear();
		}
		else if (m_vulnerable)
		{
			//CAST2_SCATTER_BREAKDOWN 이벤트에서 호출됨 : 불필요 이벤트 삭제 필요.
			m_weeper->SetState(Weeper::WEEPER_STATE::TAUNT);
			EventHandler::GetInstance()->AddEvent("WEEPER_COUNTERSTAGGER_END", 5.f, m_weeper);
			EventHandler::GetInstance()->DeleteEvent("CAST2_VULNERABLE_OFF");
			Cast2Vulnerable_OFF();
		}
		else if (m_weeper->GetState() == Weeper::WEEPER_STATE::CAST4_LOOP)
		{
			m_cast4Cancel_requiredHit -= 1;
			if (m_cast4Cancel_requiredHit > 0)
			{
				return;
			}
			else
			{
				std::cout << "canceled" << std::endl;
				Cast4Cancel_RequiredHit_To_Default();
				m_weeper->SetState(Weeper::WEEPER_STATE::CAST4_END);
				SetAIWait(false);
				EventHandler::GetInstance()->AddEvent("ANIM_END_IF_CAST4END", 4.06f, m_weeper);
				EventHandler::GetInstance()->AddEvent("MONSTER_MOBILE", 4.06f, m_weeper);
				EventHandler::GetInstance()->DeleteEvent("WEEPER_CAST4");
			}
		}
	}


}

void WeeperAI::Cast2Counter_ON()
{
	m_cast2Counter = true;
	cout << "핵폭탄 반격모드 ON" << endl;
}

void WeeperAI::Cast2Counter_OFF()
{
	m_cast2Counter = false;
	cout << "핵폭탄 반격모드 OFF" << endl;
}

void WeeperAI::Cast2Vulnerable_ON()
{
	m_vulnerable = true;
	cout << "vulnerable ON" << endl;
}

void WeeperAI::Cast2Vulnerable_OFF()
{
	m_vulnerable = false;
	cout << "vulnerable OFF" << endl;
}

void WeeperAI::Cast4Cancel_RequiredHit_To_Default()
{
	m_cast4Cancel_requiredHit = 3;
	m_weeper->m_cast4_vertVel = physx::PxVec3(0, 800, 0);
}

bool WeeperAI::DeathHandle()
{
	if (m_weeper->m_hp <= 0)
	{
		m_AIWait = true;
		m_weeper->m_currState = Weeper::WEEPER_STATE::DEATH;
		EventHandler::GetInstance()->AddEvent("ANIM_TO_WEEPER_DEAD", 4.15f, m_weeper);
		return true;
	}

	return false;
}

void WeeperAI::ReportSchedule()
{
	for (int i = 0; i< m_scheduler.size(); ++i)
	{
		std::cout << i << " : " << magic_enum::enum_name(m_scheduler[i]) << std::endl;
	}
	std::cout << std::endl << std::endl;
}

void WeeperAI::ServerMessage_SendMonsterPattern(WEEPER_SCHEDULE schedule)
{
	game::TIMER_EVENT ev{ ProtocolID::WR_MONSTER_PATTERN_ACK };
	ev.objID = m_weeper->m_id;

	if (schedule == WEEPER_SCHEDULE::CAST1)
		ev.state = magic_enum::enum_integer(server::PATTERN_TYPE::WEEPER1);
	else if (schedule == WEEPER_SCHEDULE::CAST2)
		ev.state = magic_enum::enum_integer(server::PATTERN_TYPE::WEEPER2);
	else if (schedule == WEEPER_SCHEDULE::CAST3)
		ev.state = magic_enum::enum_integer(server::PATTERN_TYPE::WEEPER3);
	else if (schedule == WEEPER_SCHEDULE::CAST4)
		ev.state = magic_enum::enum_integer(server::PATTERN_TYPE::WEEPER4);

	game::MessageHandler::GetInstance()->PushSendMessage(ev);
}
