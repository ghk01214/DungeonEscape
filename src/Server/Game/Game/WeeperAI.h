#pragma once

#include "MonsterAI.h"
#include "physx_define.h"

class Player;
class MonsterSkill;
class Weeper;
class Monster;


enum class WEEPER_SCHEDULE
{
	HOLD = 0,
	RESET = 1,				//패턴진행 직전, SetRandomTarget()실행 후, 클라이언트에서 몬스터를 돌릴 틈을 준다.
	//패턴 3번 쓰면 한번정도 섞어주는 정도	

	CAST1 = 2,		//weeper 평타
	CAST2 = 3,		//무력화 기믹. 중력장
	CAST3 = 4,		//광역 연속운석
	CAST4 = 5,		//강화 평타. 강화운석 타겟발사
	DODGE = 5		//회피동작
};

class WeeperAI : public MonsterAI
{
	friend class Weeper;
public:
	WeeperAI(Weeper* weeper);
	~WeeperAI();
public:
	void Init();
	void Update(float timeDelta);
	void LateUpdate(float timeDelta);
	void Release();
public:
	virtual void FillSchedule() override;
	virtual void ExecuteSchedule(float deltaTime) override;
	virtual bool IsEmptySchedule() override;

	virtual std::string GetRequestedScheduleName() override;

private:
	std::vector<WEEPER_SCHEDULE> m_scheduler;

private:
	Weeper* m_weeper;

	float m_detectRange;				//몬스터 인식 범위
	float m_targetPos_UpdateInterval;	//플레이어 위치 갱신 주기
};

