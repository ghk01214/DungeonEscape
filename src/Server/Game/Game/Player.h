#pragma once

#include "GameObject.h"

class CustomController;
class TriggerObject;

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
	constexpr int32_t GetHP() const { return m_hp; }

	bool IsOnGound();
	bool IsDead();

	void TriggerZoneStatusChange(server::TRIGGER_TYPE triggerType, bool status);
	void TriggerZoneStatusUpdate();
	void Trigger_Magma();
	void Trigger_Wind();
	void Trigger_SingleStrike();

	void SetAniInfo(int32_t aniIndex, float aniFrame, float aniSpeed);

	void GotHit(int32_t damage);
	void KillPlayer();
public:
	CustomController* GetController();
	void SetControllerPosition(Vec3 pos);
	Vec3 GetControllerPosition();
	Quat GetControllerRotation();
	float GetControllerMoveSpeed();
	float GetControllerJumpSpeed();
	void SetControllerMoveSpeed(float value);
	void SetControllerJumpSpeed(float value);		
	void SetControllerCameraLook(Vec3& value);

public:
	void PlayerPattern_ShootBall(server::OBJECT_TYPE type, int32_t objID, float power);
	void PlayerPattern_SingleStrike();

private:
	CustomController* m_controller = nullptr;
	TriggerObject* m_attackTrigger = nullptr;

	int32_t m_playerID = 4294967295U;

	int32_t m_aniIndex;
	float m_aniFrame;
	float m_aniSpeed;

	std::unordered_map<server::TRIGGER_TYPE, bool> m_triggerDictionary;

	int32_t m_hp;
	bool m_damaged;
	bool m_die;
};
