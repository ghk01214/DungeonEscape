#pragma once

#include "physx_define.h"

class Player;
class Monster;
class MonsterSkill;
class OverlapObject;

class MonsterAI
{
	friend class OverlapObject;
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
	void Monstermove();
	void AddSkillSize(std::string scheduleName, GeometryType shape, Vec3 size);

	virtual std::string GetRequestedScheduleName() = 0;
	physx::PxGeometry* GetRequestedSkillGeometry(std::string schedule);
	bool SkillRangeCheck();


public:
	std::vector<Player*> SkillRangeCheck_OverlapObject(std::string scheduleName);		//overlapObject 전용 호출 함수. 그 외의 클래스에서 호출하지 않는다.
	Monster* GetMonster();

public:
	void SetAIWait(bool value);
	bool GetAIWait();
	Vec3 GetTargetPosition();
	Vec3 GetTargetDir();
	physx::PxVec3 GetXZDir();
	physx::PxQuat GetRotation_For_Pattern(physx::PxVec3 xzDir);
protected:
	Monster* m_monster;
	std::vector<MonsterSkill*> m_skillSizeHolder;
	Player* m_target = nullptr;
	Vec3 m_targetPos;
	Vec3 m_targetDir;

	bool m_AIWait = false;					//카운터 대기를 위해

	float m_detectRange;					//몬스터 인식 범위
	float m_targetPos_UpdateInterval;		//플레이어 위치 갱신 주기
};

