#pragma once

#include "GameObject.h"

class CustomController;

class Player : public GameObject
{
public:
	Player(int32_t playerID = -1, const Vec3& position = Vec3(), const Quat& rotation = Quat(), const Vec3& scale = Vec3(1.f, 1.f, 1.f));
	~Player() override;

public:
	virtual void Init();
	virtual void Update(double timeDelta) override;
	virtual void LateUpdate(double timeDelta) override;
	virtual void Release();

public:
	constexpr int32_t GetPlayerID() const { return m_playerID; }
	constexpr int32_t GetAniIndex() const { return m_aniIndex; }
	constexpr float GetAniFrame() const { return m_aniFrame; }
	constexpr float GetAniSpeed() const { return m_aniSpeed; }

	bool IsOnGound();

	void SetAniInfo(int32_t aniIndex, float aniFrame, float aniSpeed);

public:
	float GetControllerMoveSpeed();
	float GetControllerJumpSpeed();
	CustomController* GetController();

	void SetControllerMoveSpeed(float value);
	void SetControllerJumpSpeed(float value);
	void SetControllerCameraLook(Vec3& value);

public:
	void PlayerPattern_ShootBall(float power);

public:
	CustomController* m_controller = nullptr;

	int32_t m_playerID = 4294967295U;

	int32_t m_aniIndex;
	float m_aniFrame;
	float m_aniSpeed;
};
