#pragma once

#include "Monster_Script.h"

class Monster_Dragon : public Monster_Script
{
private:
	enum DRAGON_STATE
	{
		ATTACK_HAND,
		ATTACK_HORN,
		ATTACK_MOUTH,
		DEFEND,
		DIE,
		GET_HIT,
		IDLE1,
		IDLE2,
		JUMP,
		RUN,
		SCREAM,
		SLEEP,
		WALK,
		WALK_BACK,
		WALK_LEFT,
		WALK_RIGHT,

		DEAD,

		END
	};

public:
	Monster_Dragon();
	Monster_Dragon(int32_t stateIndex);
	~Monster_Dragon();

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
	DRAGON_STATE m_prevState;
	DRAGON_STATE m_currState;
};