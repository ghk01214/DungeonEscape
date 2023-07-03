#include "pch.h"
#include "Event.h"
#include "SkillObject.h"

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
	executed = true;

	if (msg == "SKILL_GUIDESTART")
	{
		auto skillObj = dynamic_cast<SkillObject*>(target);
		if (skillObj)
		{
			skillObj->SetAttribute(SkillObject::SKILLATTRIBUTE::GUIDED);
		}
	}

	executed = true;
}