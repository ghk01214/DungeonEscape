#pragma once

#include "GameObject.h"
#include "physx_define.h"

class CustomController;
class Player;
class MonsterAI;

class Monster : public GameObject
{
	friend class MonsterAI;
	friend class SkillObject;
public:
	Monster(int32_t MonsterID = -1, const Vec3& position = Vec3(), const Quat& rotation = Quat(), const Vec3& scale = Vec3(1.f, 1.f, 1.f));
	~Monster() override;

public:
	virtual void Init() override;
	virtual void Update(double timeDelta) override;
	virtual void LateUpdate(double timeDelta) override;
	virtual void Release() override;

public:
	CustomController* GetController() { return m_controller; }
	Vec3 GetControllerPosition();
	void SetControllerPosition(Vec3 pos);
	int32_t GetHP() const;
	float GetAniPlayTime() const;

	void SetFBXType(server::FBX_TYPE fbxType) override;
	void SetAniPlayTime(float time);
	void SetAniEndFlag(bool flag);

	bool IsOnGround();
	bool IsDead();

public:
	float GetControllerMoveSpeed();
	float GetControllerJumpSpeed();

	void SetControllerMoveSpeed(float value);
	void SetControllerJumpSpeed(float value);
	void SetControllerCameraLook(Vec3& value);

public:
	void GetDamaged(int32_t damage);
	void SendTransform();

public:
	MonsterAI* GetAI();

public:
	//Server혼자 FSM실험을 위해 만든 임시함수. 추후 무력화 기믹에서 사용할 예정
	void SetAnimationEnd();

protected:
	CustomController* m_controller;

	int32_t m_hp;

	float m_aniPlayTime;
	bool m_aniEnd;

protected:
	MonsterAI* m_AI;
};
