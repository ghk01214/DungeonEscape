#include "pch.h"
#include "Event.h"
#include "Monster.h"
#include "Weeper.h"
#include "SkillObject.h"
#include "RigidBody.h"
#include "Collider.h"

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

	if (msg == "SKILL_AERIAL_FIRE")
	{
		auto skillObj = dynamic_cast<SkillObject*>(target);
		if (skillObj)
		{
			skillObj->SetAttribute(SkillObject::SKILLATTRIBUTE::AERIAL_FIRE, true);
			auto body = skillObj->GetComponent<RigidBody>(L"RigidBody");
			body->SetAngularDamping(0.f);
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