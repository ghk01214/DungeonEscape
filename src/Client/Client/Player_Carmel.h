﻿#pragma once

#include "MonoBehaviour.h"

class Player_Carmel : public MonoBehaviour
{
private:
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
		IDLE_A,
		IDLE_B,
		IDLE_C,
		JUMP_START,
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

		JUMPING,
		JUMP_END,
		DEAD,

		END
	};

public:
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

private:
	void CheckState();
	void UpdateFrameRepeat();
	void UpdateFrameOnce();

	void KeyInputStateChange();
	void DecidePlayerDeath();

private:
	float GetAngleBetweenVector(const XMVECTOR& vector1, const XMVECTOR& vector2);
	void TurnPlayer(Vec3 vector1, Vec3 vector2);

private:
	void MovePlayerCameraLook(void);
	void ParsePackets();

	void StartRender(network::CPacket& packet);
	void Transform(network::CPacket& packet);
	void ChangeAnimation(network::CPacket& packet);
	void AddColliderToObject(network::CPacket& packet);

private:
	PLAYER_STATE m_prevState;
	PLAYER_STATE m_currState;

	int32_t m_hp;
};