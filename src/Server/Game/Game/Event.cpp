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
		if (!executed && time < 0.f)			//RemoveReserved 비활성화, executed 거짓, 시간 음수일때만 실행
		{
			ExecuteMsg_Once();
			ExecuteMsg_continuous();
		}
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

	if (msg == "ANIM_END_IF_CAST2END")
	{
		auto weeperObj = dynamic_cast<Weeper*>(target);
		if (weeperObj)
		{
			if (weeperObj->GetState() == Weeper::WEEPER_STATE::CAST2_END)
			{
				weeperObj->SetAnimationEnd();
			}
		}
	}

	if (msg == "ANIM_END_IF_CAST4END")
	{
		auto weeperObj = dynamic_cast<Weeper*>(target);
		if (weeperObj)
		{
			if (weeperObj->GetState() == Weeper::WEEPER_STATE::CAST4_END)
			{
				weeperObj->SetAnimationEnd();
			}
		}
	}
	
	if (msg == "ANIM_TO_WEEPER_CAST4_LOOP")
	{
		auto weeperObj = dynamic_cast<Weeper*>(target);
		if (weeperObj)
		{
			weeperObj->SetState(Weeper::WEEPER_STATE::CAST4_LOOP);
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

	if (msg == "CAST2_SCATTER_AIRFIRE")											// ExecuteSchedule()의 WEEPER_SCHEDULE::CAST2:에서 호출됐음
	{															
		//원기옥 노말발사 (이 시점에서 보스는 공격받아도 핵폭탄패턴X)
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

	if (msg == "CAST2_SCATTER_BREAKDOWN")				// 주의 : 애니메이션 직접 제어라면 weeperAI::DmgCheck의 stagger에서 eventHandler에 남아있는 명령 삭제해야함.
	{
		auto skillObj = dynamic_cast<SkillObject*>(target);
		Weeper* owner = dynamic_cast<Weeper*>(skillObj->m_owner);
		WeeperAI* ownerAI = owner->GetAI();

		if (skillObj)
		{
			owner->SetState(Weeper::WEEPER_STATE::CAST2_END);						//CAST2 END로 애니메이션 진입
			if (ownerAI->m_debugmode)
				EventHandler::GetInstance()->AddEvent("ANIM_END", 4.f, owner);		//디버그 전용 애니메이션 종료 코드
			EventHandler::GetInstance()->AddEvent("AI_WAIT_FREE", 6.f, owner);

			EventHandler::GetInstance()->AddEvent("CAST2_VULNERABLE_ON", 4.f, owner);		//애니메이션 종료 시간쯤 Vulnerable 1.5초 정도 ON
			EventHandler::GetInstance()->AddEvent("CAST2_VULNERABLE_OFF", 5.5f, owner);

			skillObj->SetRemoveReserved();											//Ascend 속성의 오브젝트는 삭제

			int scatterCnt = 50;
			float scatterTime = 4.f;
			for (int i = 0; i < scatterCnt; ++i)
			{
				static std::uniform_real_distribution<float> distribution(0.f, scatterTime);
			
				EventHandler::GetInstance()->AddEvent("WEEPER_CAST2_SCATTER_FUNCTIONCALL", distribution(dre), owner);
			}

			//addevent(카운터 진입시간)
			//addevent(카운터 탈출시간)
		}
	}

	if (msg == "CAST2_NUCLEAR_AIRFIRE")											// Weeper::Pattern_Cast2_CounterNuclear()에 의해 호출되었다. 
	{																			// 해당 함수에서 반격모드 해제 + 애니메이션 CAST2 END로 변경했음.
		auto scatterEvent = EventHandler::GetInstance()->GetEvent("CAST2_SCATTER_AIRFIRE");
		auto skillObj = static_cast<SkillObject*>(scatterEvent->target);
		Weeper* weeper = static_cast<Weeper*>(target);
		WeeperAI* weeperAI = weeper->GetAI();

		EventHandler::GetInstance()->DeleteEvent("CAST2_SCATTER_AIRFIRE");		// 1. Cast2 노말 진행 이벤트 삭제
		//weeper->SetState(Weeper::WEEPER_STATE::CAST2_END);					// 2. Cast2 원기옥 던지기 애니메이션 재생
		if (weeperAI->m_debugmode)
			EventHandler::GetInstance()->AddEvent("ANIM_END", 4.f, weeper);		//디버그 전용 애니메이션 종료 코드
		EventHandler::GetInstance()->AddEvent("AI_WAIT_FREE", 6.f, weeper);		//AI wait 해제 (던지는 애니메이션이 끝나고 몇초 후 이동 가능하도록)
		EventHandler::GetInstance()->AddEvent("ANIM_END_IF_CAST2END", 6.f, weeper);		//클라에서 애니메이션 종료 못들어도 종료

		skillObj->WeeperNuclearFire();
		skillObj->SetAttribute(SkillObject::SKILLATTRIBUTE::NUCLEAR, true);
		skillObj->SetSkillObjectType(SkillObject::SKILLOBJECTTYPE::WEEPER_CAST2_BALL_NUCLEAR);
		}

	if (msg == "WEEPER_COUNTERSTAGGER_END")											// WeeperAI::DmgCheck()에서 호출
	{																				// 카운터 당하고, n초후 state를 IDLE로 회복
		Weeper* weeper = dynamic_cast<Weeper*>(target);
		if (weeper)
		{
			if (weeper->GetState() == Weeper::WEEPER_STATE::TAUNT)
				weeper->SetState(Weeper::WEEPER_STATE::IDLE);
		}
	}

	if (msg == "CAST2_VULNERABLE_ON")											
	{																				
		Weeper* weeper = dynamic_cast<Weeper*>(target);
		if (weeper)
		{
			WeeperAI* weeperAI = weeper->GetAI();
			weeperAI->Cast2Vulnerable_ON();
		}
	}

	if (msg == "CAST2_VULNERABLE_OFF")
	{
		Weeper* weeper = dynamic_cast<Weeper*>(target);
		if (weeper)
		{
			WeeperAI* weeperAI = weeper->GetAI();
			weeperAI->Cast2Vulnerable_OFF();
		}
	}

	if (msg == "WEEPER_CAST3_FUNCTIONCALL")
	{
		auto weeperObj = dynamic_cast<Weeper*>(target);
		if (weeperObj)
		{
			weeperObj->Pattern_Cast3();
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

	if (msg == "SKILL_LEVITATE_END")
	{
		auto skillObj = dynamic_cast<SkillObject*>(target);
		if (skillObj)
		{
			skillObj->SetAttribute(SkillObject::SKILLATTRIBUTE::LEVITATE, false);
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

void Event::ExecuteMsg_continuous()
{
	if (msg == "WEEPER_CAST4")		//cast4 start 애니메이션 종료 후 호출. 애니메이션은 이미 Cast4_Loop으로 전환됐음.
	{
		auto weeperObj = dynamic_cast<Weeper*>(target);
		executed = false;
		if (!weeperObj)
			return;

		bool over = weeperObj->Pattern_Cast4();
		if (over)
		{
			weeperObj->GetAI()->Cast4Cancel_RequiredHit_To_Default();			//cast4 캔슬횟수 초기화 + cast4 velocity 초기화 + 
			weeperObj->SetState(Weeper::WEEPER_STATE::CAST4_END);
			EventHandler::GetInstance()->AddEvent("ANIM_END_IF_CAST4END", 4.06f, target);
			executed = true;	//이벤트 소멸
		}
	}
}
