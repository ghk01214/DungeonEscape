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

	if (msg == "WEEPER_CAST1_FUNCTIONCALL")
	{
		auto weeperObj = dynamic_cast<Weeper*>(target);
		if (weeperObj)
		{
			weeperObj->Pattern_Cast1();
		}
	}

	if (msg == "SKILL_ASCENDSTART_NORMAL")								//위로 발사(노말)
	{
		auto skillObj = dynamic_cast<SkillObject*>(target);
		if (skillObj)
		{
			skillObj->SetAttribute(SkillObject::SKILLATTRIBUTE::ASCENDING, true);
			Weeper* owner = dynamic_cast<Weeper*>(skillObj->m_owner);
			owner->SetState(Weeper::WEEPER_STATE::CAST2_END);
			
			WeeperAI* ownerAI = owner->GetAI();
			if (ownerAI->m_debugmode)
				EventHandler::GetInstance()->AddEvent("ANIM_END", 4.f, owner);		//디버그 모드일경우 CAST2 애니메이션 강제종료
			owner->GetAI()->SetAIWait(true);										//카운터 시간부여를 위한 밑작업
		}
	}

	if (msg == "SKILL_ASCENDSTART_EXPlOSION")							//위로 발사(핵폭탄)
	{
		auto skillObj = dynamic_cast<SkillObject*>(target);
		if (skillObj)
		{
			// delete event SKILL_ASCENDSTART_NORMAL.
			skillObj->SetAttribute(SkillObject::SKILLATTRIBUTE::ASCENDING, true);
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

	executed = true;
}