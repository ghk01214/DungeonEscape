#pragma once

#include "Monster.h"

class WeeperAI;
class SkillObject;
class Player;

class Weeper : public Monster
{
	friend class WeeperAI;
	friend class SkillObject;
public:
	enum WEEPER_STATE
	{
		CAST1,				 // 6.36(공격 0.82 ~ 2.0)
		CAST2_END,			 // 1.87(공격 0.24 ~ 0.89)
		CAST2_LOOP,			 // 2.20
		CAST2_START,		 // 2.20
		CAST3,				 // 7.36(공격 0.85 ~ 2.0)
		CAST4_END,			 // 4.03
		CAST4_LOOP,			 // 3.03
		CAST4_START,		 // 2.7
		DAMAGE,				 // 2.37
		DEATH,				 // 4.19
		DODGE,				 // 2.03
		IDLE,				 // 6.36
		IDLE_BREAK,
		STATUE1,
		STATUE2,
		STATUE3,
		TAUNT,
		TURN_LEFT,
		TURN_RIGHT,
		WALK,
		WALK_BACK,
		WALK_BACK_NO_LEGS,

		DEAD,

		END
	};

public:
	Weeper(int32_t MonsterID = -1, const Vec3& position = Vec3(), const Quat& rotation = Quat(), const Vec3& scale = Vec3(1.f, 1.f, 1.f));
	~Weeper() override;

public:
	void Init() override;
	void Update(double timeDelta) override;
	void LateUpdate(double timeDelta) override;
	void Release() override;

public:
	void CheckState();
	void UpdateFrameOnce();

public:
	WeeperAI* GetAI();

public:
	void Pattern_Cast1();				//공 3개 발사

	// * Cast2 연계는 Event에서 주로 이루어진다.
	SkillObject* Pattern_Cast2();					//원기옥 생성
	void Pattern_Cast2_Scatter();					//원기옥 파편 1개 랜덤위치 생성
	void Pattern_Cast2_CounterNuclear();

	SkillObject* Pattern_Cast3();
	bool Pattern_Cast4();							//패턴이 끝나면 true 반환
	void Pattern_Cast4_Effect(Player* player);		//패턴이 끝나고 플레이어에게 적용할 효과.

public:
	int Randnum_Cast1_XInterval();
	float Randnum_Cast1_YInterval();
	physx::PxVec3 Randnum_Cast2_XZInterval();

public:
	WEEPER_STATE GetState() const;
	void SetState(WEEPER_STATE state);

	void SendChangedStateAgain();

private:
	physx::PxVec3 m_cast4_vertVel = physx::PxVec3(0, 1000, 0);

private:
	WEEPER_STATE m_prevState;
	WEEPER_STATE m_currState;

	int32_t m_sendState;

	WeeperAI* m_weeperAI;
};
