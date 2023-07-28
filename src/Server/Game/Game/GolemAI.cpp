#include "pch.h"
#include "GolemAI.h"
#include "Golem.h"
#include "Monster.h"
#include "CustomController.h"
#include "RigidBody.h"
#include "SkillObject.h"
#include "EventHandler.h"
#include "Transform.h"
#include "MessageHandler.h"

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
	AddSkillSize("ATTACK1", GeometryType::Box, Vec3(200, 200, 300), false);				//어퍼컷
	AddSkillSize("ATTACK2", GeometryType::Box, Vec3(200, 500, 300), false);				//앞펀치
	AddSkillSize("ATTACK3", GeometryType::Box, Vec3(200, 500, 300), false);				//찍기
	AddSkillSize("ATTACK4", GeometryType::Box, Vec3(300, 200, 300), false);				//일반 휘두르기
	//AddSkillSize("ATTACK4", GeometryType::Box, Vec3(1000, 1000, 500), false);			//돌진확인을 위한 디버그용값

	AddSkillSize("ROAR", GeometryType::Sphere, Vec3(1000, 1000, 1000), false);			//포효
	AddSkillSize("RUN", GeometryType::Box, Vec3(250, 250, 250), true);					//돌진
	AddSkillSize("JUMP", GeometryType::Box, Vec3(1000, 1000, 1000), false);				//돌진
	AddSkillSize("SPELL", GeometryType::Sphere, Vec3(1500, 1500, 1500), true);			//차징 충격파 공격
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
	//m_scheduler.emplace_back(GOLEM_SCHEDULE::ATTACK3);
	//m_scheduler.emplace_back(GOLEM_SCHEDULE::ATTACK4);
	//m_scheduler.emplace_back(GOLEM_SCHEDULE::ROAR);
	//m_scheduler.emplace_back(GOLEM_SCHEDULE::RUN);
	//m_scheduler.emplace_back(GOLEM_SCHEDULE::JUMP);
	//m_scheduler.emplace_back(GOLEM_SCHEDULE::SPELL);

	std::cout << "Filled Schedule" << std::endl;
	ReportSchedule();
}

void GolemAI::ExecuteSchedule(float deltaTime)
{
	if (m_golem->m_currState != Golem::GOLEM_STATE::IDLE1 && m_golem->m_currState != Golem::GOLEM_STATE::WALK)
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

				QuatUpdateForClient();

				//실제로 공격판정이 들어가는건 애니메이션이 살짝 진행된 후
				m_golem->m_currState = Golem::GOLEM_STATE::ATTACK1;											//STATE : ATTACK1로 변경
				EventHandler::GetInstance()->AddEvent("GOLEM_ATTACK1_FUNCTIONCALL", 0.5f, m_golem);			//0.5초 후 overlapObj 활성화
				EventHandler::GetInstance()->AddEvent("OVERLAPOBJECT_DEACTIVATE", 1.5f, m_golem);			//1.5초 후	overlapObj 비활성화(+중복목록 초기화)
				EventHandler::GetInstance()->AddEvent("ANIM_END", 1.5f, m_golem);							//			같은 시간에 애니메이션 종료
				//STATE : IDLE로 변경은 STATE_CHECK에서

				SetAIWait(true);
				EventHandler::GetInstance()->AddEvent("AI_WAIT_FREE", 1.7f, m_golem);						//같은 시간에 애니메이션 종료
			}
			else
			{
				GolemMove();
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

				QuatUpdateForClient();

				//실제로 공격판정이 들어가는건 애니메이션이 살짝 진행된 후
				m_golem->m_currState = Golem::GOLEM_STATE::ATTACK2;											//STATE : ATTACK1로 변경
				EventHandler::GetInstance()->AddEvent("GOLEM_ATTACK2_FUNCTIONCALL", 0.5f, m_golem);			//0.5초 후 overlapObj 활성화
				EventHandler::GetInstance()->AddEvent("OVERLAPOBJECT_DEACTIVATE", 1.5f, m_golem);			//1.5초 후	overlapObj 비활성화(+중복목록 초기화)
				EventHandler::GetInstance()->AddEvent("ANIM_END", 1.5f, m_golem);							//			같은 시간에 애니메이션 종료
				//STATE : IDLE로 변경은 STATE_CHECK에서

				SetAIWait(true);
				EventHandler::GetInstance()->AddEvent("AI_WAIT_FREE", 2.f, m_golem);						//같은 시간에 애니메이션 종료
			}
			else
			{
				GolemMove();
			}
		}
		break;

		case GOLEM_SCHEDULE::ATTACK3:
		{
			inSkillRange = SkillRangeCheck();
			if (inSkillRange)
			{
				m_scheduler.erase(m_scheduler.begin());
				ReportSchedule();

				QuatUpdateForClient();

				//실제로 공격판정이 들어가는건 애니메이션이 살짝 진행된 후
				m_golem->m_currState = Golem::GOLEM_STATE::ATTACK3;											//STATE : ATTACK1로 변경
				EventHandler::GetInstance()->AddEvent("GOLEM_ATTACK3_FUNCTIONCALL", 0.5f, m_golem);			//0.5초 후 overlapObj 활성화
				EventHandler::GetInstance()->AddEvent("OVERLAPOBJECT_DEACTIVATE", 1.5f, m_golem);			//1.5초 후	overlapObj 비활성화(+중복목록 초기화)
				EventHandler::GetInstance()->AddEvent("ANIM_END", 2.33f, m_golem);							//			같은 시간에 애니메이션 종료
				//STATE : IDLE로 변경은 STATE_CHECK에서

				SetAIWait(true);
				EventHandler::GetInstance()->AddEvent("AI_WAIT_FREE", 2.5f, m_golem);						//같은 시간에 애니메이션 종료
			}
			else
			{
				GolemMove();
			}
		}
		break;

		case GOLEM_SCHEDULE::ATTACK4:
		{
			inSkillRange = SkillRangeCheck();
			if (inSkillRange)
			{
				m_scheduler.erase(m_scheduler.begin());
				ReportSchedule();

				QuatUpdateForClient();

				//실제로 공격판정이 들어가는건 애니메이션이 살짝 진행된 후
				m_golem->m_currState = Golem::GOLEM_STATE::ATTACK4;											//STATE : ATTACK1로 변경
				EventHandler::GetInstance()->AddEvent("GOLEM_ATTACK4_FUNCTIONCALL", 0.5f, m_golem);			//0.5초 후 overlapObj 활성화
				EventHandler::GetInstance()->AddEvent("OVERLAPOBJECT_DEACTIVATE", 1.4f, m_golem);			//1.5초 후	overlapObj 비활성화(+중복목록 초기화)
				EventHandler::GetInstance()->AddEvent("ANIM_END", 1.6f, m_golem);							//			같은 시간에 애니메이션 종료
				//STATE : IDLE로 변경은 STATE_CHECK에서

				SetAIWait(true);
				EventHandler::GetInstance()->AddEvent("AI_WAIT_FREE", 1.9f, m_golem);						//같은 시간에 애니메이션 종료
			}
			else
			{
				GolemMove();
			}
		}
		break;

		case GOLEM_SCHEDULE::ROAR:
		{
			inSkillRange = SkillRangeCheck();
			if (inSkillRange)
			{
				m_scheduler.erase(m_scheduler.begin());
				ReportSchedule();

				QuatUpdateForClient();

				//실제로 공격판정이 들어가는건 애니메이션이 살짝 진행된 후
				m_golem->m_currState = Golem::GOLEM_STATE::ROAR;											//STATE : ATTACK1로 변경
				EventHandler::GetInstance()->AddEvent("GOLEM_ROAR_FUNCTIONCALL", 0.5f, m_golem);			//0.5초 후 overlapObj 활성화
				EventHandler::GetInstance()->AddEvent("OVERLAPOBJECT_DEACTIVATE", 2.33f, m_golem);			//1.5초 후	overlapObj 비활성화(+중복목록 초기화)
				EventHandler::GetInstance()->AddEvent("ANIM_TO_GOLEM_IDLE", 2.33f, m_golem);				//			같은 시간에 애니메이션 종료

				SetAIWait(true);
				EventHandler::GetInstance()->AddEvent("AI_WAIT_FREE", 2.6f, m_golem);						//같은 시간에 애니메이션 종료
			}
			else
			{
				GolemMove();
			}
		}
		break;

		case GOLEM_SCHEDULE::RUN:
		{
			if (GetXZDistance() < 2000.f)
			{
				m_scheduler.erase(m_scheduler.begin());
				ReportSchedule();

				QuatUpdateForClient();

				SetAIWait(true);																			//패턴시작 : AI 대기

				m_golem->m_currState = Golem::GOLEM_STATE::WALK;											//STATE : WALK으로 변경
				EventHandler::GetInstance()->AddEvent("GOLEM_MOVE", 0.1f, m_golem);							//이동명령 시작 (Event는 Continous 속성)

				EventHandler::GetInstance()->AddEvent("ANIM_TO_GOLEM_RUN_LOOP", 1.f, m_golem);				//STATE : RUN으로 변경
				EventHandler::GetInstance()->AddEvent("GOLEM_SET_FASTSPEED", 1.f, m_golem);				//controller의 속도를 빠르게
				EventHandler::GetInstance()->AddEvent("GOLEM_RUN_FUNCTIONCALL", 1.f, m_golem);				//overlapObj 활성화 시작

				EventHandler::GetInstance()->AddEvent("ANIM_TO_GOLEM_WALK", 4.f, m_golem);					//STATE : WALK으로 변경
				EventHandler::GetInstance()->AddEvent("GOLEM_SET_ORIGINALSPEED", 4.f, m_golem);				//controller의 속도 정상으로
				EventHandler::GetInstance()->AddEvent("OVERLAPOBJECT_DEACTIVATE", 4.f, m_golem);			//overlapObj 비활성화 적용

				EventHandler::GetInstance()->AddEvent("ANIM_TO_GOLEM_IDLE", 4.5f, m_golem);					//STATE : IDLE로 변경
				EventHandler::GetInstance()->AddEvent("GOLEM_MOVE_STOP", 4.5f, m_golem);					//이동명령 정지.


				EventHandler::GetInstance()->AddEvent("AI_WAIT_FREE", 4.5f, m_golem);						//패턴종료 : AI 재개
			}
			else
			{
				GolemMove();
			}
		}
		break;

		case GOLEM_SCHEDULE::JUMP:
		{
			inSkillRange = SkillRangeCheck();
			if (inSkillRange)
			{
				m_scheduler.erase(m_scheduler.begin());
				ReportSchedule();

				QuatUpdateForClient();

				SetAIWait(true);

				m_golem->m_currState = Golem::GOLEM_STATE::ROAR;
				EventHandler::GetInstance()->AddEvent("ANIM_TO_GOLEM_JUMPSTART", 2.33, m_golem);
				EventHandler::GetInstance()->AddEvent("GOLEM_JUMP_FUNCTIONCALL", 2.83, m_golem);			//애니메이션 JumpStart의 0.5초에서 상승힘 적용

				EventHandler::GetInstance()->AddEvent("GOLEM_SELECT_LAND_FUNCTIONCALL", 4.83, m_golem);		//위치선정 + 경고장판 + 애니메이션 JUMPLOOP으로
				EventHandler::GetInstance()->AddEvent("ANIM_TO_GOLEM_JUMPLOOP", 4.83, m_golem);

				EventHandler::GetInstance()->AddEvent("GOLEM_JUMP_DESCEND_FUNCTIONCALL", 6.83, m_golem);	//경고장판 애니메이션이 끝나면 새로운 위치로 이동 + 하강힘 적용
				EventHandler::GetInstance()->AddEvent("GOLEM_JUMP_LANDCHECK_FUNCTIONCALL", 6.83, m_golem);	//랜딩체크. continous속성
			}
			else
			{
				GolemMove();
			}
		}
		break;

		case GOLEM_SCHEDULE::SPELL:
		{
			inSkillRange = SkillRangeCheck();
			if (inSkillRange)
			{
				m_scheduler.erase(m_scheduler.begin());
				ReportSchedule();

				QuatUpdateForClient();

				SetAIWait(true);

				m_golem->m_currState = Golem::GOLEM_STATE::ROAR;

				EventHandler::GetInstance()->AddEvent("ANIM_TO_GOLEM_SPELL_START", 2.33f, m_golem);			//ROAR > SPELL_START 진입

				EventHandler::GetInstance()->AddEvent("ANIM_TO_GOLEM_SPELL_LOOP", 3.83f, m_golem);			//SPELL START > SPELL_LOOP 진입
				EventHandler::GetInstance()->AddEvent("SPELL_VULNERABLE_ON", 8.83f, m_golem);				//카운터 가능시간 1초
				EventHandler::GetInstance()->AddEvent("SPELL_VULNERABLE_OFF", 9.83f, m_golem);				//카운터 시간을 놓치면 SPELL_END진입

				EventHandler::GetInstance()->AddEvent("ANIM_TO_GOLEM_SPELL_END", 9.83f, m_golem);			//SPELL LOOP > SPELL_END 진입

				EventHandler::GetInstance()->AddEvent("GOLEM_SPELL_FUNCTIONCALL", 10.36f, m_golem);			//0.53초 후 overlapObj 활성화
				EventHandler::GetInstance()->AddEvent("OVERLAPOBJECT_DEACTIVATE", 11.36f, m_golem);			//활성화 시간은 1초						대지충격파 유지시간 수정은 여기서
				EventHandler::GetInstance()->AddEvent("ANIM_END_IF_SPELL_END", 11.33f, m_golem);			//SPELL END > IDLE1 진입

				EventHandler::GetInstance()->AddEvent("AI_WAIT_FREE", 13.33f, m_golem);						//IDLE진입 후 2초정도 있다가 AI재개

			}
			else
			{
				GolemMove();
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
		if (m_vulnerable)
		{
			m_golem->SetState(Golem::GOLEM_STATE::DAMAGE);
			EventHandler::GetInstance()->DeleteEvent("ANIM_TO_GOLEM_SPELL_END");
			EventHandler::GetInstance()->DeleteEvent("GOLEM_SPELL_FUNCTIONCALL");
			EventHandler::GetInstance()->DeleteEvent("OVERLAPOBJECT_DEACTIVATE");
			EventHandler::GetInstance()->DeleteEvent("ANIM_END_IF_SPELL_END");
			EventHandler::GetInstance()->DeleteEvent("AI_WAIT_FREE");				//기존 명령 삭제

			EventHandler::GetInstance()->AddEvent("ANIM_TO_GOLEM_STUN", 1.33f, m_golem);		//DAMAGE애니메이션 1.33초후 STUN으로 변경
			EventHandler::GetInstance()->AddEvent("ANIM_END", 2.33f * 3.f, m_golem);			//STUN은 6.99초 유지
			EventHandler::GetInstance()->AddEvent("AI_WAIT_FREE", 2.33f * 3.f, m_golem);		//STUN이 끝나면 aiwait해제
			m_vulnerable = false;
		}
	}

}

void GolemAI::Vulnuerable_Set(bool value)
{
	m_vulnerable = value;
	if (value)
		cout << "vulnerable ON" << endl;
	else
		cout << "vulnerable OFF" << endl;
}

void GolemAI::GolemMove()
{
	Monstermove();
	m_golem->SetState(Golem::GOLEM_STATE::WALK);
}

void GolemAI::ReportSchedule()
{
	for (int i = 0; i < m_scheduler.size(); ++i)
	{
		cout << i << " : " << magic_enum::enum_name(m_scheduler[i]) << endl;
	}
	std::cout << std::endl << std::endl;
}
