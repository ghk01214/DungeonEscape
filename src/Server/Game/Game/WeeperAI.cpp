#include "pch.h"
#include "WeeperAI.h"
#include "Weeper.h"
#include "Monster.h"
#include "CustomController.h"

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
	AddSkillSize("CAST1", GeometryType::Box, Vec3(50, 50, 300));		//z거리 200
	AddSkillSize("CAST2", GeometryType::Box, Vec3(50, 50, 200));
	AddSkillSize("CAST3", GeometryType::Box, Vec3(50, 50, 400));
	AddSkillSize("CAST4", GeometryType::Box, Vec3(50, 50, 800));

	m_weeper->SetControllerMoveSpeed(10.f);

	//Schedule 추가
	m_scheduler.emplace_back(WEEPER_SCHEDULE::CAST1);
}

void WeeperAI::Update(float timeDelta)
{
	MonsterAI::Update(timeDelta);
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
	m_scheduler.emplace_back(WEEPER_SCHEDULE::CAST1);
	m_scheduler.emplace_back(WEEPER_SCHEDULE::CAST2);
	m_scheduler.emplace_back(WEEPER_SCHEDULE::CAST3);
	m_scheduler.emplace_back(WEEPER_SCHEDULE::CAST4);
	std::cout << "Filled Schedule" << std::endl;
	ReportSchedule();
}

void WeeperAI::ExecuteSchedule(float deltaTime)
{
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
				m_weeper->Pattern_Cast1();
				m_weeper->m_currState = Weeper::CAST1;
				m_scheduler.erase(m_scheduler.begin());
				ReportSchedule();

			}
			else
			{
				m_weeper->GetController()->MonsterMove(TO_PX3(m_targetDir));
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

void WeeperAI::ReportSchedule()
{
	for (int i = 0; i< m_scheduler.size(); ++i)
	{
		std::cout << i << " : " << magic_enum::enum_name(m_scheduler[i]) << std::endl;
	}
	std::cout << std::endl << std::endl;
}
