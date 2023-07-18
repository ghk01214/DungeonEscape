#include "pch.h"
#include "GolemAI.h"
#include "Golem.h"
#include "Monster.h"
#include "CustomController.h"
#include "SkillObject.h"
#include "EventHandler.h"

using namespace std;

GolemAI::GolemAI(Golem* golem) :
	MonsterAI(golem),
	m_golem(golem)
{
}

GolemAI::~GolemAI()
{
}

void GolemAI::Init()
{
	MonsterAI::Init();

	//SkillSize 추가			//triggerObject의 2/3의 크기면 적당함
	AddSkillSize("ATTACK1", GeometryType::Box, Vec3(200, 200, 300));				//어퍼컷
	AddSkillSize("ATTACK2", GeometryType::Box, Vec3(200, 500, 300));				//앞펀치
	//AddSkillSize("ATTACK3", GeometryType::Box, Vec3(400, 400, 400));				//찍기
	//AddSkillSize("ATTACK4", GeometryType::Box, Vec3(50, 50, 450));					//일반 휘두르기
	//
	//AddSkillSize("ROAR", GeometryType::Box, Vec3(50, 50, 450));					//포효
	//AddSkillSize("SPELL", GeometryType::Box, Vec3(50, 50, 450));				//차지 휘두르기 (skillObject 넣어도 될거같다)

	m_golem->SetControllerMoveSpeed(10.f);
}

void GolemAI::Update(float timeDelta)
{
	MonsterAI::Update(timeDelta);

	auto state = m_golem->m_currState;	//debg

	DamageCheck();
}

void GolemAI::LateUpdate(float timeDelta)
{
	MonsterAI::LateUpdate(timeDelta);
}

void GolemAI::Release()
{
	MonsterAI::Release();
}

void GolemAI::FillSchedule()
{
	if (!m_target)
		return;		// 초기 SetRandomTarget이 실패할 경우 탈출

	m_scheduler.emplace_back(GOLEM_SCHEDULE::ATTACK1);
	m_scheduler.emplace_back(GOLEM_SCHEDULE::ATTACK2);


	std::cout << "Filled Schedule" << std::endl;
	ReportSchedule();
}

void GolemAI::ExecuteSchedule(float deltaTime)
{
	if (m_golem->m_currState != Golem::GOLEM_STATE::IDLE1 && m_golem->m_currState != Golem::GOLEM_STATE::IDLE2)
		return;			// 기본스탠딩, 이동도중에만 패턴수행이 가능하다.

	static float directionUpdateInterval = 0.f;
	if (directionUpdateInterval > 1.0f)
	{
		UpdateTargetPos();
		directionUpdateInterval = 0.f;
	}
	directionUpdateInterval += deltaTime;

	GOLEM_SCHEDULE schedule = m_scheduler[0];

	bool inSkillRange = false;

	switch (schedule)
	{
		case GOLEM_SCHEDULE::ATTACK1:
		{
			inSkillRange = SkillRangeCheck();
			if (inSkillRange)
			{
				m_scheduler.erase(m_scheduler.begin());
				ReportSchedule();

				//실제로 공격판정이 들어가는건 애니메이션이 살짝 진행된 후
				m_golem->m_currState = Golem::GOLEM_STATE::ATTACK1;											//STATE : ATTACK1로 변경
				EventHandler::GetInstance()->AddEvent("GOLEM_ATTACK1_FUNCTIONCALL", 0.5f, m_golem);			//0.5초 후 overlapObj 활성화
				EventHandler::GetInstance()->AddEvent("OVERLAPOBJECT_DEACTIVATE", 1.5f, m_golem);			//1.5초 후	overlapObj 비활성화(+중복목록 초기화)
				EventHandler::GetInstance()->AddEvent("ANIM_END", 1.5f, m_golem);							//			같은 시간에 애니메이션 종료 
																											//STATE : IDLE로 변경은 STATE_CHECK에서 

				SetAIWait(true);
				EventHandler::GetInstance()->AddEvent("AI_WAIT_FREE", 2.f, m_golem);						//같은 시간에 애니메이션 종료 
			}
			else
			{
				Monstermove();
				m_golem->m_currState = Golem::GOLEM_STATE::IDLE1;
			}
		}
		break;

		case GOLEM_SCHEDULE::ATTACK2:
		{
			inSkillRange = SkillRangeCheck();
			if (inSkillRange)
			{
				m_scheduler.erase(m_scheduler.begin());
				ReportSchedule();

				//실제로 공격판정이 들어가는건 애니메이션이 살짝 진행된 후
				m_golem->m_currState = Golem::GOLEM_STATE::ATTACK1;											//STATE : ATTACK1로 변경
				EventHandler::GetInstance()->AddEvent("GOLEM_ATTACK2_FUNCTIONCALL", 0.5f, m_golem);			//0.5초 후 overlapObj 활성화
				EventHandler::GetInstance()->AddEvent("OVERLAPOBJECT_DEACTIVATE", 1.5f, m_golem);			//1.5초 후	overlapObj 비활성화(+중복목록 초기화)
				EventHandler::GetInstance()->AddEvent("ANIM_END", 1.5f, m_golem);							//			같은 시간에 애니메이션 종료 
				//STATE : IDLE로 변경은 STATE_CHECK에서 

				SetAIWait(true);
				EventHandler::GetInstance()->AddEvent("AI_WAIT_FREE", 2.f, m_golem);						//같은 시간에 애니메이션 종료 
			}
			else
			{
				Monstermove();
				m_golem->m_currState = Golem::GOLEM_STATE::IDLE1;
			}
		}
		break;
	}
}

bool GolemAI::IsEmptySchedule()
{
	if (m_scheduler.empty())
		return true;

	return false;
}

std::string GolemAI::GetRequestedScheduleName()
{
	if (m_scheduler.empty())
		return NULL;

	GOLEM_SCHEDULE schedule = m_scheduler[0];
	std::string name = magic_enum::enum_name(schedule).data();
	return name;
}

void GolemAI::DamageCheck()
{
	static int originalHealth = m_golem->GetHP();

	if (originalHealth > m_golem->GetHP())
	{
		originalHealth = m_golem->GetHP();
	//	if (m_cast2Counter)
	//	{
	//		m_weeper->Pattern_Cast2_CounterNuclear();
	//	}
	}

}

void GolemAI::ReportSchedule()
{
	for (int i = 0; i < m_scheduler.size(); ++i)
	{
		cout << i << " : " << magic_enum::enum_name(m_scheduler[i]) << endl;
	}
	std::cout << std::endl << std::endl;
}
