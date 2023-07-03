#pragma once

#include "Monster_Script.h"

class Monster_Scorpion : public Monster_Script
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
	Monster_Scorpion();
	Monster_Scorpion(int32_t stateIndex);
	~Monster_Scorpion();

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
	SCORPION_STATE m_prevState;
	SCORPION_STATE m_currState;
};