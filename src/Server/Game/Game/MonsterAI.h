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
	void Monstermove_ignoreWait();
	void AddSkillSize(std::string scheduleName, GeometryType shape, Vec3 size, bool centerBox);
	void QuatUpdateForClient();

	virtual std::string GetRequestedScheduleName() = 0;
	physx::PxGeometry* GetRequestedSkillGeometry(std::string schedule);
	MonsterSkill* GetRequestedMonsterSkill(std::string schedule);
	bool SkillRangeCheck();
	float GetXZDistance();


public:
	std::vector<Player*> SkillRangeCheck_OverlapObject(std::string scheduleName, OverlapObject* overlapObj);		//overlapObject 전용 호출 함수. 그 외의 클래스에서 호출하지 않는다.
	Monster* GetMonster();

public:
	void MonsterStart();						//거리내에 플레이어가 감지되면 target값을 채우고 ai 업데이트 시작.

	void SetAIWait(bool value);
	bool GetAIWait();
	Vec3 GetTargetPosition();
	Vec3 GetTargetDir();
	Player* GetTarget();
	physx::PxVec3 GetXZDir();
	physx::PxVec3 GetReverseXZDir();
	physx::PxVec3 GetOldXZDir();
	physx::PxQuat GetRotation_For_Pattern(physx::PxVec3 xzDir);

protected:
	void BossPatternUIStart(server::TRIGGER_INTERACTION_TYPE type);

protected:
	Monster* m_monster;
	std::vector<MonsterSkill*> m_skillSizeHolder;
	Player* m_target = nullptr;
	Vec3 m_targetPos;
	Vec3 m_targetDir;
	Vec3 m_monsterLook;

	bool m_AIWait = true;

	float m_detectRange;					//몬스터 인식 범위
	float m_targetPos_UpdateInterval;		//플레이어 위치 갱신 주기

	bool m_start = false;					//작동시작
	float m_checkRange;
};

