#pragma once

#include "Monster_Script.h"

class OneTimeDialogue_Script;

class Monster_Weeper : public Monster_Script
{
public:
	enum WEEPER_STATE
	{
		CAST1,
		CAST2_END,
		CAST2_LOOP,
		CAST2_START,
		CAST3,
		CAST4_END,
		CAST4_LOOP,
		CAST4_START,
		DAMAGE,
		DEATH,
		DODGE,
		IDLE,
		IDLE_BREAK,
		STATUE1,
		STATUE2,
		STATUE3,
		TAUNT,		// 놀리다, 비웃다
		TURN_LEFT,
		TURN_RIGHT,
		WALK,
		WALK_BACK,
		WALK_BACK_NO_LEGS,

		DEAD,

		END
	};

public:
	Monster_Weeper();
	Monster_Weeper(int32_t stateIndex);
	~Monster_Weeper();

public:
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

private:
	void CheckState() override;
	void UpdateFrameRepeat() override;
	void UpdateFrameOnce() override;

	void ParsePackets();

public:
	void SetDialogue(std::shared_ptr<OneTimeDialogue_Script> script);

private:
	WEEPER_STATE m_prevState;
	WEEPER_STATE m_currState;

	std::shared_ptr<OneTimeDialogue_Script> m_cast4UI;
};