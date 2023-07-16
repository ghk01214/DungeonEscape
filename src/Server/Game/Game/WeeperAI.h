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

	CAST1 = 2,				//평타 (약한 유도 3개)
	CAST2 = 3,				//하누마탄 원기옥
	CAST3 = 4,				//광역 연속운석
	CAST4 = 5,				//강화 평타. 강화운석 타겟발사
	DODGE = 5				//회피동작
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

public:
	void DamageCheck();
	void Cast2Counter_ON();
	void Cast2Counter_OFF();
	void Cast2Vulnerable_ON();
	void Cast2Vulnerable_OFF();
	void Cast4Cancel_RequiredHit_To_Default();

private:
	void ReportSchedule();		//콘솔출력 목적의 디버그 전용 함수

private:
	std::vector<WEEPER_SCHEDULE> m_scheduler;

private:
	Weeper* m_weeper;

	float m_detectRange;				//몬스터 인식 범위
	float m_targetPos_UpdateInterval;	//플레이어 위치 갱신 주기
	float m_cast2Counter = false;		//Cast2의 nuclear
	float m_vulnerable = false;			//Cast2의 카운터 취약 순간
	float m_cast4Cancel_requiredHit = 3;	//Cast4를 중단시키기 위한 피격 횟수

public:
	bool m_debugmode = false;			//서버 FSM 확인용 변수
};

