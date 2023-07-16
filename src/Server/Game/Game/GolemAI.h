﻿#pragma once

#include "MonsterAI.h"
#include "physx_define.h"

class Player;
class MonsterSkill;
class Golem;
class Monster;


enum class GOLEM_SCHEDULE
{
	HOLD = 0,
	ATTACK1 = 1,
	ATTACK2 = 2,
	ATTACK3 = 3,
	ATTACK4 = 4,
};

class GolemAI : public MonsterAI
{
	friend class Golem;
public:
	GolemAI(Golem* golem);
	~GolemAI();
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
	//void Cast2Counter_ON();
	//void Vulnuerable_ON();
	//..

private:
	void ReportSchedule();		//콘솔출력 목적의 디버그 전용 함수

private:
	std::vector<GOLEM_SCHEDULE> m_scheduler;

private:
	Golem* m_golem;

public:
	bool m_debugmode = true;			//서버 FSM 확인용 변수
};
