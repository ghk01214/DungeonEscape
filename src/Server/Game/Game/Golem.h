#pragma once
#pragma once

class Monster;

class Golem : public Monster
{
public:
	enum GOLEM_STATE
	{
		ROAR,			   // 2.33
		SPELL_END,		   // 1.5
		SPELL_LOOP,		   // 1.5
		SPELL_START,	   // 1.5
		STUN,			   // 2.33
		ATTACK1,		   // 1.5
		ATTACK2,		   // 1.53
		ATTACK3,		   // 2.33
		ATTACK4,		   // 1.6
		DAMAGE,			   // 1.33
		READY,			   // 2.33
		DEATH,			   // 3.3
		LAUGH,			   // 3
		TALK,			   // 2.97
		IDLE1,			   // 2
		IDLE2,			   // 3
		JUMP_END,		   // 1.27
		JUMP_LOOP,		   // 1
		JUMP_START,		   // 1.27
		RUN_BACKWARD,	   // 2
		RUN,			   // 2
		RUN_LEFT,		   // 2
		RUN_RIGHT,		   // 2
		SWIM_BACKWARD,	   // 2
		SWIM,			   // 2
		SWIM_IDLE,		   // 2.33
		SWIM_LEFT,		   // 2
		SWIM_RIGHT,		   // 2
		WALK_BACKWARD,	   // 2.33
		WALK,			   // 2.33
		WALK_LEFT,		   // 2.33
		WALK_RIGHT,		   // 2.33

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

	void SendChangedStateAgain();

private:
	GOLEM_STATE m_prevState;
	GOLEM_STATE m_currState;

	int32_t m_sendState;

private:
	Vec3 m_patternSize;
};
