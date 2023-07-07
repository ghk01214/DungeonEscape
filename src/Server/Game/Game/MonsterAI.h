﻿#pragma once

#include "physx_define.h"

class Player;
class Monster;
class MonsterSkill;

class MonsterAI
{
public:
	MonsterAI(Monster* monster);
	~MonsterAI();
public:
	void Init();
	void Update(float timeDelta);
	void LateUpdate(float timeDelta);
	void Release();
public:
	virtual void FillSchedule() = 0;
	virtual void ExecuteSchedule(float deltaTime) = 0;
	virtual bool IsEmptySchedule() = 0;
public:
	void SetRandomTarget();		//타겟을 변경
	void UpdateTargetPos();		//0.n초 주기로 타겟 플레이어 위치 갱신 (플레이어 일직선 추적 방지)
	void AddSkillSize(std::string scheduleName, GeometryType shape, Vec3 size);

	virtual std::string GetRequestedScheduleName() = 0;
	physx::PxGeometry* GetRequestedSkillGeometry(std::string schedule);
	bool SkillRangeCheck();

protected:
	Monster* m_monster;
	std::vector<MonsterSkill*> m_skillSizeHolder;
	Player* m_target = nullptr;
	Vec3 m_targetPos;
	Vec3 m_targetDir;

	float m_detectRange;				//몬스터 인식 범위
	float m_targetPos_UpdateInterval;	//플레이어 위치 갱신 주기
};

