#pragma once

#include "MonoBehaviour.h"

class Player_Mistic : public MonoBehaviour
{
	enum PLAYER_STATE
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
	virtual void Update() override;
	virtual void LateUpdate() override;

private:
	float GetAngleBetweenVector(const XMVECTOR& vector1, const XMVECTOR& vector2);
	void TurnPlayer(Vec3 vector1, Vec3 vector2);

private:
	void MovePlayerCameraLook(void);

private:
	float		_speed = 500.f;
	PLAYER_STATE _playerState = PLAYER_STATE::END;
};