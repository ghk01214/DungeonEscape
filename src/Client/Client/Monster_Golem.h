#pragma once

#include "Monster_Script.h"

class Monster_Golem : public Monster_Script
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
	Monster_Golem();
	Monster_Golem(int32_t stateIndex);
	~Monster_Golem();

public:
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

private:
	void CheckState() override;
	void UpdateFrameRepeat() override;
	void UpdateFrameOnce() override;

	void ParsePackets();

private:
	GOLEM_STATE m_prevState;
	GOLEM_STATE m_currState;
};