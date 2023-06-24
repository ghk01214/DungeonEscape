#pragma once

#include "GameObject.h"

class CustomController;

class Monster : public GameObject
{
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

protected:
	CustomController* m_controller;

	int32_t m_hp;

	float m_aniPlayTime;
	bool m_aniEnd;
};
