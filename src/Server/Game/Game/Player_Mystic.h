#pragma once

#include "Player.h"

class Player_Mystic : public Player
{
	enum Mystic_State
	{
		AERT,
		ATK0,
		ATK1,
		ATK2,
		ATK3,
		ATK4,
		BLOACK,
		DAMAGE,
		DATH,
		DIE0,
		DIE1,
		DIE2,
		IDLE_A,
		IDLE_B,
		IDLE_C,
		JUMP,
		MOVE,
		MOVE_L,
		MOVE_R,
		REST,
		RUN,
		RUN_L,
		RUN_R,
		SHOOT,
		SLEEP,
		SWOON,
		TIRED,
		VICTORY_A,
		VICTORY_B,
		WALK,
		WALK_L,
		WALK_R,
		END
	};
public:
	Player_Mystic();
	~Player_Mystic() override;

public:
	virtual void Init();
	virtual void Update(double timeDelta) override;
	virtual void LateUpdate(double timeDelta) override;
	virtual void Release();
};

