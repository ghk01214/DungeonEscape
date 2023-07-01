#pragma once

class Monster;

class Golem : public Monster
{
public:
	enum GOLEM_STATE
	{
		ROAR,
		SPELL_END,
		SPELL_LOOP,
		SPELL_START,
		STUN,
		ATTACK1,
		ATTACK2,
		ATTACK3,
		ATTACK4,
		DAMAGE,
		READY,
		DEATH,
		LAUGH,
		TALK,
		IDLE1,
		IDLE2,
		JUMP_END,
		JUMP_LOOP,
		JUMP_START,
		RUN_BACKWARD,
		RUN,
		RUN_LEFT,
		RUN_RIGHT,
		SWIM_BACKWARD,
		SWIM,
		SWIM_IDLE,
		SWIM_LEFT,
		SWIM_RIGHT,
		WALK_BACKWARD,
		WALK,
		WALK_LEFT,
		WALK_RIGHT,

		DEAD,

		END
	};

public:
	Golem(int32_t MonsterID = -1, const Vec3& position = Vec3(), const Quat& rotation = Quat(), const Vec3& scale = Vec3(1.f, 1.f, 1.f));
	~Golem() override;

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
	GOLEM_STATE GetState() const;

	void SetState(GOLEM_STATE state);

private:
	GOLEM_STATE m_prevState;
	GOLEM_STATE m_currState;

private:
	Vec3 m_patternSize;
};
