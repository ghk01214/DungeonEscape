#include "pch.h"
#include "Event.h"
#include "Player.h"
#include "Monster.h"
#include "MonsterAI.h"
#include "Weeper.h"
#include "WeeperAI.h"
#include "Golem.h"
#include "GolemAI.h"
#include "SkillObject.h"
#include "RigidBody.h"
#include "Collider.h"
#include "CustomController.h"
#include "EventHandler.h"
#include "TriggerObject2.h"

Event::Event(std::string context, double remainTime, GameObject* subject) :
	msg(context), time(remainTime), target(subject)
{
}

Event::~Event()
{
}

void Event::Tick(double deltaTime)
{
	if (executed)
		return;

	time -= deltaTime;

	//if (msg == "ANIM_END_IF_SPELL_END")
	//{
	//	std::cout << "remaining TIme : " << time << std::endl;
	//}
}

void Event::Tick_TimeInterval()
{
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
	//공용
	if (msg == "ANIM_END")
	{
		auto monsterObj = dynamic_cast<Monster*>(target);
		if (monsterObj)
		{
			monsterObj->SetAnimationEnd();
		}
	}

	if (msg == "MONSTER_MOBILE")
	{
		auto monsterObj = dynamic_cast<Monster*>(target);
		if (monsterObj)
		{
			monsterObj->SetMonsterMobile();
		}
	}

	if (msg == "AI_WAIT_FREE")
	{
		auto monsterObj = dynamic_cast<Monster*>(target);
		if (monsterObj)
		{
			auto monsterAI = monsterObj->GetAI();
			monsterAI->SetAIWait(false);
			monsterObj->SetMonsterMobile();
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
			skillObj->SetAttribute(SkillObject::SKILLATTRIBUTE::NEW_LEVITATE, false);
			skillObj->SetAttribute(SkillObject::SKILLATTRIBUTE::LEVITATE, true);
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

	if (msg == "TIMER")
	{
		std::cout << "timer" << std::endl;
	}


	//player
	if (msg == "PLAYER_STUN_OFF")
	{
		auto playerObj = dynamic_cast<Player*>(target);
		if (playerObj)
		{
			playerObj->SetStun(false);
		}
	}

	if (msg == "METEOR_AVAILABLE_SET_TRUE")
	{
		auto playerObj = dynamic_cast<Player*>(target);
		if (playerObj)
		{
			playerObj->SetMeteorAttackAvailable(true);
			std::cout << "메테오 재충전" << std::endl;
		}
	}

	if (msg == "REMOVE_PLAYER_METEOR")
	{
		auto meteorObj = dynamic_cast<SkillObject*>(target);
		if (meteorObj)
		{
			meteorObj->SetRemoveReserved();
			meteorObj->ServerMessage_SkillRemove();
		}
	}

	if (msg == "PLAYER_OVERLAPOBJ_ACTIVATE")
	{
		auto playerObj = dynamic_cast<Player*>(target);
		if (playerObj)
		{
			playerObj->Set_OverlapObject(true);

			//MessageHandler
		}
	}

	if (msg == "PLAYER_OVERLAPOBJ_DEACTIVATE")
	{
		auto playerObj = dynamic_cast<Player*>(target);
		if (playerObj)
		{
			playerObj->Set_OverlapObject(false);
		}
	}


	//weeper
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
			
			EventHandler::GetInstance()->AddEvent("ANIM_END", 1.87f, owner);		//디버그 전용 애니메이션 종료 코드


			EventHandler::GetInstance()->AddEvent("CAST2_VULNERABLE_ON", 4.f, owner);		//애니메이션 종료 시간쯤 Vulnerable 1.5초 정도 ON
			EventHandler::GetInstance()->AddEvent("CAST2_VULNERABLE_OFF", 5.5f, owner);
			EventHandler::GetInstance()->AddEvent("AI_WAIT_FREE", 5.7f, owner);

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

		EventHandler::GetInstance()->DeleteEvent("CAST2_SCATTER_AIRFIRE");		// Cast2 노말 진행 이벤트 삭제
	
		EventHandler::GetInstance()->AddEvent("ANIM_END", 1.87f, weeper);			//디버그 전용 애니메이션 종료 코드
		EventHandler::GetInstance()->AddEvent("AI_WAIT_FREE", 3.f, weeper);			//AI wait 해제 (던지는 애니메이션이 끝나고 몇초 후 이동 가능하도록)
		//EventHandler::GetInstance()->AddEvent("ANIM_END_IF_CAST2END", 6.f, weeper);		//클라에서 애니메이션 종료 못들어도 종료

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
			{
				weeper->SetState(Weeper::WEEPER_STATE::IDLE);
				weeper->SetMonsterMobile();
			}
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



	//golem
	if (msg == "GOLEM_ATTACK1_FUNCTIONCALL")
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (golemObj)
		{
			golemObj->Pattern_Attack1();
		}
	}

	if (msg == "GOLEM_ATTACK2_FUNCTIONCALL")
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (golemObj)
		{
			golemObj->Pattern_Attack2();
		}
	}

	if (msg == "GOLEM_ATTACK3_FUNCTIONCALL")
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (golemObj)
		{
			golemObj->Pattern_Attack3();
		}
	}

	if (msg == "GOLEM_ATTACK4_FUNCTIONCALL")
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (golemObj)
		{
			golemObj->Pattern_Attack4();
		}
	}

	if (msg == "GOLEM_ROAR_FUNCTIONCALL")
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (golemObj)
		{
			golemObj->Pattern_Roar();
		}
	}

	if (msg == "GOLEM_RUN_FUNCTIONCALL")
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (golemObj)
		{
			golemObj->Pattern_Run();
		}
	}

	if (msg == "GOLEM_SPELL_FUNCTIONCALL")
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (golemObj)
		{
			golemObj->Pattern_Spell();
		}
	}

	if (msg == "OVERLAPOBJECT_DEACTIVATE")
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (golemObj)
		{
			golemObj->OverlapObject_Deactivate();
		}
	}

	if (msg == "GOLEM_JUMP_FUNCTIONCALL")
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (golemObj)
		{
			golemObj->Pattern_Jump_Ascend();
		}
	}

	if (msg == "GOLEM_SELECT_LAND_FUNCTIONCALL")
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (golemObj)
		{
			golemObj->Pattern_Jump_Select_LandPosition();
		}
	}

	if (msg == "GOLEM_JUMP_DESCEND_FUNCTIONCALL")
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (golemObj)
		{
			golemObj->Pattern_Jump_Descend();
		}
	}

	if (msg == "ANIM_TO_GOLEM_IDLE")
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (golemObj)
		{
			golemObj->SetState(Golem::GOLEM_STATE::IDLE1);
		}
	}

	if (msg == "ANIM_TO_GOLEM_WALK")
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (golemObj)
		{
			golemObj->SetState(Golem::GOLEM_STATE::WALK);
		}
	}

	if (msg == "ANIM_TO_GOLEM_RUN_LOOP")
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (golemObj)
		{
			golemObj->SetState(Golem::GOLEM_STATE::RUN);
		}
	}

	if (msg == "ANIM_TO_GOLEM_JUMPSTART")
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (golemObj)
		{
			golemObj->SetState(Golem::GOLEM_STATE::JUMP_START);
		}
	}

	if (msg == "ANIM_TO_GOLEM_JUMPLOOP")
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (golemObj)
		{
			golemObj->SetState(Golem::GOLEM_STATE::JUMP_LOOP);
		}
	}

	if (msg == "ANIM_TO_GOLEM_SPELL_START")
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (golemObj)
		{
			golemObj->SetState(Golem::GOLEM_STATE::SPELL_START);
		}
	}

	if (msg == "ANIM_TO_GOLEM_SPELL_LOOP")
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (golemObj)
		{
			golemObj->SetState(Golem::GOLEM_STATE::SPELL_LOOP);
		}
	}

	if (msg == "ANIM_TO_GOLEM_SPELL_END")
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (golemObj)
		{
			golemObj->SetState(Golem::GOLEM_STATE::SPELL_END);
		}
	}

	if (msg == "ANIM_TO_GOLEM_STUN")
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (golemObj)
		{
			golemObj->SetState(Golem::GOLEM_STATE::STUN);
		}
	}

	if (msg == "ANIM_END_IF_SPELL_END")
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (golemObj)
		{
			if (golemObj->GetState() == Golem::GOLEM_STATE::SPELL_END)
			{
				golemObj->SetState(Golem::GOLEM_STATE::IDLE1);
			}
		}
	}

	if (msg == "GOLEM_SET_ORIGINALSPEED")
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (golemObj)
		{
			golemObj->GetController()->SetMoveSpeedScale(1.f);
		}
	}

	if (msg == "GOLEM_SET_FASTSPEED")
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (golemObj)
		{
			// 달릴 때 이동속도
			golemObj->GetController()->SetMoveSpeedScale(2.f);
		}
	}

	if (msg == "GOLEM_MOVE_STOP")
	{
		EventHandler::GetInstance()->DeleteEvent("GOLEM_MOVE");
	}

	if (msg == "SPELL_VULNERABLE_ON")
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (golemObj)
		{
			golemObj->GetAI()->Vulnuerable_Set(true);
		}
	}

	if (msg == "SPELL_VULNERABLE_OFF")
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (golemObj)
		{
			golemObj->GetAI()->Vulnuerable_Set(false);
		}
	}


	// PORTAL
	if (msg == "PORTAL1" || msg == "PORTAL2" || msg == "PORTAL3" || msg == "PORTAL4" || msg == "PORTAL5" || msg == "PORTAL6" || msg == "PORTAL7" || msg == "PORTAL8")
	{
		auto triggerObj = dynamic_cast<TriggerObject2*>(target);
		if (triggerObj)
		{
			triggerObj->SendPlayers();
		}
	}


	// ARTIFACT			 
	if (msg == "PILLAR_EFFECT_OFF")
	{
		//클라이언트로 PILLAR에 있는 마법 이펙트를 끄도록 명령
	}

	if (msg == "CAMERA_BACKTO_PLAYER")
	{
		//클라이언트로 카메라를 다시 플레이어를 비추도록 수정
	}

	executed = true;
}

void Event::ExecuteMsg_continuous()
{
	//exectued = false로 값을 부여안하면 바로 event가 삭제된다. update에서 executeMsg를 우선 실행하기 때문이다.

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
			weeperObj->GetAI()->SetAIWait(false);
			weeperObj->SetState(Weeper::WEEPER_STATE::CAST4_END);
			EventHandler::GetInstance()->AddEvent("ANIM_END_IF_CAST4END", 4.06f, target);
			EventHandler::GetInstance()->AddEvent("MONSTER_MOBILE", 4.06f, target);
			executed = true;	//이벤트 소멸
		}
	}




	if (msg == "GOLEM_MOVE")		//cast4 start 애니메이션 종료 후 호출. 애니메이션은 이미 Cast4_Loop으로 전환됐음.
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (!golemObj)
			return;

		executed = false;
		golemObj->GetAI()->Monstermove_ignoreWait();
	}

	if (msg == "GOLEM_JUMP_LANDCHECK_FUNCTIONCALL")		//Jump에서 착지까지 반복호출
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (!golemObj)
			return;

		executed = false;
		bool land = golemObj->Pattern_Jump_LandCheck();
		if (land)												//착지했다면
		{
			golemObj->SetState(Golem::GOLEM_STATE::JUMP_END);	//착지애니메이션 재생

			EventHandler::GetInstance()->AddEvent("ANIM_END", 1.3, golemObj);			//착지애니메이션이 끝났다면 IDLE로
			EventHandler::GetInstance()->AddEvent("AI_WAIT_FREE", 2.3, golemObj);		//AI 재개
			//golemObj->GetAI()->SetAIWait(false);
			executed = true;
		}
	}

	if (msg == "GOLEM_PHYSICLAYER_TO_DEFAULT")		//Jump에서 착지까지 반복호출
	{
		auto golemObj = dynamic_cast<Golem*>(target);
		if (!golemObj)
			return;

		executed = false;
		bool complete = golemObj->PhysicLayer_SetToDefault();
		if (complete)
			executed = true;
	}

	if (msg == "GOLEM_ATTACK3_STUN_APPLY")		//Jump에서 착지까지 반복호출
	{
		auto playerObj = dynamic_cast<Player*>(target);
		if (!playerObj)
			return;

		executed = false;
		bool ground = playerObj->GetController()->IsOnGround();
		if (ground)
		{
			playerObj->SetState(Player::PLAYER_STATE::DAMAGE);
			executed = true;
		}
	}

	if (msg == "GOLEM_ATTACK_DAMAGE_APPLY")		//Jump에서 착지까지 반복호출
	{
		auto playerObj = dynamic_cast<Player*>(target);
		if (!playerObj)
			return;

		executed = false;
		bool ground = playerObj->GetController()->IsOnGround();
		if (ground)
		{
			if (playerObj->GetState() == Player::PLAYER_STATE::DAMAGE)
				playerObj->SetState(Player::PLAYER_STATE::IDLE1);
			else
				playerObj->SetState(Player::PLAYER_STATE::DAMAGE);

			executed = true;
		}
	}
}


