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
			auto weeperAI = monsterObj->GetAI();
			weeperAI->SetAIWait(false);
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

			// * WeeperAI::Execute(Cast2)에서 반격준비 기능을 구현하고, 여기에서는, 반격준비 기능을 해제해야한다.
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

			for (int i = 0; i < 200; ++i)
			{
				static std::uniform_real_distribution<float> distribution(0.f, 5.f);
				EventHandler::GetInstance()->AddEvent("WEEPER_CAST2_SCATTER_FUNCTIONCALL", distribution(dre), owner);
				//반격기능 ONOFF
			}

			//addevent(카운터 진입시간)
			//addevent(카운터 탈출시간)
		}
	}

	if (msg == "SKILL_ASCENDSTART_EXPlOSION")							//위로 발사(핵폭탄)
	{
		auto skillObj = dynamic_cast<SkillObject*>(target);
		if (skillObj)
		{
			//Weeper* owner = dynamic_cast<Weeper*>(skillObj->m_owner);
			//WeeperAI* ownerAI = owner->GetAI();

			//EventHandler::GetInstance()->DeleteEvent("SKILL_ASCENDSTART_NORMAL");		//정상 발사 예정인 스킬을 취소.

			//skillObj->SetAttribute(SkillObject::SKILLATTRIBUTE::ASCENDING, true);
			//owner->SetState(Weeper::WEEPER_STATE::CAST2_END);							//STATE : CAST2 END로 설정
			//if (ownerAI->m_debugmode)
			//	EventHandler::GetInstance()->AddEvent("ANIM_END", 4.f, owner);			//디버그 모드일경우 애니메이션 종료시간 설정

			//owner->GetAI()->SetAIWait(true);											//카운터 시간부여를 위한 밑작업

			//EventHandler::GetInstance()->AddEvent("SKILL_REMOVE", 8.f, owner);

	
			//n초후 ascend속성 삭제, descend attribute enchant
			
		}
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