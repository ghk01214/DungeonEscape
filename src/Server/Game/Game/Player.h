#pragma once

#include "GameObject.h"

class CustomController;
class TriggerObject;

class Player : public GameObject
{
public:
	enum PLAYER_STATE
	{
		AERT,
		ATK0,
		ATK1,
		ATK2,
		ATK3,
		ATK4,
		BLOCK,
		DAMAGE,
		DASH,
		DIE0,
		DIE1,
		DIE2,
		IDLE1,
		IDLE2,
		IDLE3,
		JUMP_START,
		MOVE,
		MOVE_LEFT,
		MOVE_RIGHT,
		REST,
		RUN,
		RUN_LEFT,
		RUN_RIGHT,
		SHOOT,
		SLEEP,
		SWOON,			// 기절
		TIRED,
		VICTORY1,
		VICTORY2,
		WALK,
		WALK_LEFT,
		WALK_RIGHT,

		JUMPING,
		JUMP_END,
		DEAD,

		END
	};

public:
	Player(int32_t playerID = -1, const Vec3& position = Vec3(), const Quat& rotation = Quat(), const Vec3& scale = Vec3(1.f, 1.f, 1.f));
	~Player() override;

public:
	virtual void Init();
	virtual void Update(double timeDelta) override;
	virtual void LateUpdate(double timeDelta) override;
	virtual void Release();

public:
	constexpr int32_t GetHP() const { return m_hp; }

	void IsOnGound();
	bool IsDead();

	void TriggerZoneStatusChange(server::TRIGGER_TYPE triggerType, bool status);
	void TriggerZoneStatusUpdate();
	void Trigger_Magma();
	void Trigger_Wind();
	void Trigger_SingleStrike();

	PLAYER_STATE GetState() const;
	float GetAniPlayTime() const;

	void SetState(PLAYER_STATE state);
	void SetState(int32_t index);
	void SetAniPlayTime(float time);
	void ChangeAniEndFlag(bool flag);

	void GetDamaged(int32_t damage);

public:
	void ChangeStateByKeyInput();
	void CheckState();
	void UpdateFrame();

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

private:
	void PlayerPattern_ShootBall(server::OBJECT_TYPE type, float power);
	void PlayerPattern_SingleStrike();
	int32_t IsAttackKeyDown();

	void SendTransform();

private:
	CustomController* m_controller = nullptr;
	TriggerObject* m_attackTrigger = nullptr;

	PLAYER_STATE m_prevState;
	PLAYER_STATE m_currState;

	float m_aniPlayTime;
	bool m_aniEnd;

	std::unordered_map<server::TRIGGER_TYPE, bool> m_triggerDictionary;

	int32_t m_hp;

	bool m_firstSingleStrike;
};
