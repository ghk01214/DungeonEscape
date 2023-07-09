#include "pch.h"
#include "Event.h"
#include "Monster.h"
#include "MonsterAI.h"
#include "Weeper.h"
#include "WeeperAI.h"
#include "SkillObject.h"
#include "RigidBody.h"
#include "Collider.h"
#include "EventHandler.h"

Event::Event(std::string context, float remainTime, GameObject* subject) :
	msg(context), time(remainTime), target(subject)
{
}

Event::~Event()
{
}

void Event::Tick(float deltaTime)
{
	if (executed)
		return;

	time -= deltaTime;

	if (!target->GetRemoveReserved())
	{
		if (!executed && time <0.f)			//RemoveReserved 비활성화, executed 거짓, 시간 음수일때만 실행
			ExecuteMsg_Once();					
	}
}

void Event::ExecuteMsg_Once()
{
	if (msg == "ANIM_END")
	{
		auto monsterObj = dynamic_cast<Monster*>(target);
		if (monsterObj)
		{
			monsterObj->SetAnimationEnd();
		}
	}

	if (msg == "AI_WAIT_FREE")
	{
		auto monsterObj = dynamic_cast<Monster*>(target);
		if (monsterObj)
		{
			auto monsterAI = monsterObj->GetAI();
			monsterAI->SetAIWait(false);
		}
	}


	if (msg == "WEEPER_CAST1_FUNCTIONCALL")
	{
		auto weeperObj = dynamic_cast<Weeper*>(target);
		if (weeperObj)
		{
			weeperObj->Pattern_Cast1();
		}
	}

	if (msg == "WEEPER_CAST2_SCATTER_FUNCTIONCALL")
	{
		auto weeperObj = dynamic_cast<Weeper*>(target);
		if (weeperObj)
		{
			weeperObj->Pattern_Cast2_Scatter();
		}
	}

	if (msg == "CAST2_SCATTER_AIRFIRE")											//원기옥 노말발사 (이 시점에서 보스는 공격받아도 핵폭탄패턴X)
	{
		auto skillObj = dynamic_cast<SkillObject*>(target);
		Weeper* owner = dynamic_cast<Weeper*>(skillObj->m_owner);
		WeeperAI* ownerAI = owner->GetAI();
		if (skillObj)
		{
			ownerAI->Cast2Counter_OFF();
			skillObj->SetAttribute(SkillObject::SKILLATTRIBUTE::ASCENDING, true);
			EventHandler::GetInstance()->AddEvent("CAST2_SCATTER_BREAKDOWN", 3.f, target);		//3초후 SkillObject삭제 및 Scatter공격시작. 애니메이션 CAST2_END로 진입
		}
	}

	if (msg == "CAST2_SCATTER_BREAKDOWN")
	{
		auto skillObj = dynamic_cast<SkillObject*>(target);
		Weeper* owner = dynamic_cast<Weeper*>(skillObj->m_owner);
		WeeperAI* ownerAI = owner->GetAI();

		if (skillObj)
		{
			owner->SetState(Weeper::WEEPER_STATE::CAST2_END);						//CAST2 END로 애니메이션 진입
			if (ownerAI->m_debugmode)
				EventHandler::GetInstance()->AddEvent("ANIM_END", 4.f, owner);		//디버그 전용 애니메이션 종료 코드
			EventHandler::GetInstance()->AddEvent("AI_WAIT_FREE", 4.f, owner);

			skillObj->SetRemoveReserved();											//Ascend 속성의 오브젝트는 삭제

			int scatterCnt = 70;
			float scatterTime = 5.f;
			for (int i = 0; i < scatterCnt; ++i)
			{
				static std::uniform_real_distribution<float> distribution(0.f, scatterTime);
			
				EventHandler::GetInstance()->AddEvent("WEEPER_CAST2_SCATTER_FUNCTIONCALL", distribution(dre), owner);
			}

			//addevent(카운터 진입시간)
			//addevent(카운터 탈출시간)
		}
	}

	if (msg == "CAST2_NUCLEAR_AIRFIRE")											// Weeper::Pattern_Cast2_CounterNuclear()에 의해 호출되었다. 반격모드는 해제됐음.
	{
		auto scatterEvent = EventHandler::GetInstance()->GetEvent("CAST2_SCATTER_AIRFIRE");
		auto skillObj = static_cast<SkillObject*>(scatterEvent->target);
		Weeper* weeper = static_cast<Weeper*>(target);
		WeeperAI* weeperAI = weeper->GetAI();

		EventHandler::GetInstance()->DeleteEvent("CAST2_SCATTER_AIRFIRE");		// 1. Cast2 노말 진행 이벤트 삭제
		weeper->SetState(Weeper::WEEPER_STATE::CAST2_END);						// 2. Cast2 원기옥 던지기 애니메이션 재생
		if (weeperAI->m_debugmode)
			EventHandler::GetInstance()->AddEvent("ANIM_END", 4.f, weeper);		//디버그 전용 애니메이션 종료 코드
		EventHandler::GetInstance()->AddEvent("AI_WAIT_FREE", 6.f, weeper);		//AI wait 해제 (던지는 애니메이션이 끝나고 몇초 후 이동 가능하도록)

		skillObj->WeeperNuclearFire();
		}

	if (msg == "CAST2_NUCLEAR_TEST")
	{
		auto skillObj = static_cast<SkillObject*>(target);
		skillObj->WeeperNuclearFire();
		std::cout << "디버그 코드 실행완료" << std::endl;
	}

	if (msg == "SKILL_ASCENDSTART_EXPlOSION")							//위로 발사(핵폭탄)
	{
		std::cout << "예전 코드 찾아라. 엉뚱한 이벤트를 호출하는 놈이 있다." << std::endl;
	}

	if (msg == "SKILL_RELEASE")
	{
		auto skillObj = dynamic_cast<SkillObject*>(target);
		if (skillObj)
		{
			skillObj->SetRemoveReserved();
		}
	}

	if (msg == "SKILL_GUIDESTART")
	{
		auto skillObj = dynamic_cast<SkillObject*>(target);
		if (skillObj)
		{
			skillObj->SetAttribute(SkillObject::SKILLATTRIBUTE::GUIDED, true);
			auto body = skillObj->GetComponent<RigidBody>(L"RigidBody");
			body->SetAngularDamping(0.f);
		}
	}

	if (msg == "SKILL_REMOVE")
	{
		auto skillObj = dynamic_cast<SkillObject*>(target);
		if (skillObj)
		{
			skillObj->SetRemoveReserved();
		}
	}

	executed = true;
}