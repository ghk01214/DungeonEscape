#pragma once

#include "GameObject.h"

class CustomController;

class Monster : public GameObject
{
public:
	Monster(int32_t MonsterID = -1, const Vec3& position = Vec3(), const Quat& rotation = Quat(), const Vec3& scale = Vec3(1.f, 1.f, 1.f));
	~Monster() override;

public:
	virtual void Init();
	virtual void Update(double timeDelta) override;
	virtual void LateUpdate(double timeDelta) override;
	virtual void Release();

public:
	constexpr int32_t GetMonsterID() const { return m_MonsterID; }
	constexpr int32_t GetAniIndex() const { return m_aniIndex; }
	constexpr float GetAniFrame() const { return m_aniFrame; }
	CustomController* GetController() { return m_controller; }
	constexpr int32_t GetHP() const { return m_hp; }

	void SetAniInfo(int32_t aniIndex, float aniFrame, float aniSpeed);

	bool IsOnGround();
	bool IsDead();

public:
	float GetControllerMoveSpeed();
	float GetControllerJumpSpeed();

	void SetControllerMoveSpeed(float value);
	void SetControllerJumpSpeed(float value);
	void SetControllerCameraLook(Vec3& value);

public:
	void MonsterPattern_GroundHit();	//땅을 내리쳐 같은 메쉬에 서있는 오브젝트들을 공중에 띄운다
	void GotHit(int32_t damage);

public:
	CustomController* m_controller = nullptr;

	int32_t m_MonsterID = 4294967295U;

	int32_t m_hp;

	int32_t m_aniIndex;
	float m_aniFrame;
	float m_aniSpeed;
};
