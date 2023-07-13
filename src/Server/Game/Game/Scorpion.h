#pragma once

class Monster;

class Scorpion : public Monster
{
public:
	enum SCORPION_STATE
	{
		COMBAT_DEATH,
		STUN,
		SUBMERGE_ATTACK_LEFT,
		SUBMERGE_ATTACK_MIDDLE,
		SUBMERGE_ATTACK_RIGHT,
		ATTACK1,
		ATTACK2,
		ATTACK3,
		ATTACK4,
		ATTACK_SPECIAL,
		DODGE_LEFT,
		DODGE_RIGHT,
		DAMAGE,
		READY,
		DEATH,
		IDLE1,
		IDLE2,
		IDLE3,
		IDLE_DAMAGE,
		JUMP_END,
		JUMP_LOOP,
		JUMP_START,
		RUN,
		RUN_BACKWARD,
		RUN_LEFT,
		RUN_RIGHT,
		SUBMERGE_END,
		SUBMERGE_LOOP,
		SUBMERGE_START,
		SWIM,
		SWIM_BACKWARD,
		SWIM_DEATH,
		SWIM_IDLE,
		SWIM_LEFT,
		SWIM_RIGHT,
		WALK,
		WALK_BACKWARD,
		WALK_LEFT,
		WALK_RIGHT,

		DEAD,

		END
	};

public:
	Scorpion(int32_t MonsterID = -1, const Vec3& position = Vec3(), const Quat& rotation = Quat(), const Vec3& scale = Vec3(1.f, 1.f, 1.f));
	~Scorpion() override;

public:
	void Init() override;
	void Update(double timeDelta) override;
	void LateUpdate(double timeDelta) override;
	void Release() override;

public:
	void CheckState();
	void UpdateFrameRepeat();
	void UpdateFrame();

public:
	SCORPION_STATE GetState() const;

	void SetState(SCORPION_STATE state);

	void SendChangedStateAgain();

private:
	SCORPION_STATE m_prevState;
	SCORPION_STATE m_currState;

	int32_t m_sendState;

private:
	Vec3 m_patternSize;
};
