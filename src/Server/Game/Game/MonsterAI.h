#pragma once

#include "physx_define.h"

class Player;

enum class SCHEDULE
{
	HOLD = 0,
	RESET = 1,				//패턴진행 직전, SetRandomTarget()실행 후, 클라이언트에서 몬스터를 돌릴 틈을 준다.
							//패턴 3번 쓰면 한번정도 섞어주는 정도	

	WEEPER_CAST1 = 2,		//weeper 평타
	WEEPER_CAST2 = 3,		//무력화 기믹. 중력장
	WEEPER_CAST3 = 4,		//광역 연속운석
	WEEPER_CAST4 = 5,		//강화 평타. 강화운석 타겟발사
	WEEPER_DODGE = 5
};

class MonsterAI
{
	friend class Monster;

public:
	MonsterAI(Monster* monster);
	~MonsterAI();
public:
	void Init();
	void Update();
	void Late_Update();
	void Release();
public:
	void SchedulerUpdate();
public:
	void SetRandomTarget();		//타겟을 변경
	void UpdateTargetPos();		//0.n초 주기로 타겟 플레이어 위치 갱신 (플레이어 일직선 추적 방지)
	void AddSkillSize(GeometryType type, Vec3 size, const std::wstring& skillTag);
	bool SkillRangeCheck();

	physx::PxGeometry* GetSkillGeometry();

private:
	std::vector<SCHEDULE> m_scheduler;

private:
	Monster* m_monster;
	std::unordered_map<std::wstring, physx::PxGeometry*> m_skillSizeHolder;
	Player* m_target;
	Vec3 m_targetPos;
	Vec3 m_targetDir;

	float m_detectRange;				//몬스터 인식 범위
	float m_targetPos_UpdateInterval;	//플레이어 위치 갱신 주기
};

