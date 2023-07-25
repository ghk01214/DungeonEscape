﻿#pragma once

#include "MonoBehaviour.h"

class Player_Script : public MonoBehaviour
{
public:
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
		IDLE1,
		IDLE2,
		IDLE3,
		JUMP_START,
		MOVE,
		MOVE_LEFT,
		MOVE_RIGHT,
		REST,
		RUN,
		RUN_LEFT,
		RUN_RIGHT,
		SHOOT,
		SLEEP,
		SWOON,			// 기절
		TIRED,
		VICTORY1,
		VICTORY2,
		WALK,
		WALK_LEFT,
		WALK_RIGHT,

		JUMPING,
		JUMP_END,
		DEAD,

		END
	};

public:
	Player_Script() = default;
	Player_Script(server::FBX_TYPE type, int32_t state = 12);
	~Player_Script();

public:
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

private:
	void CheckState();
	void UpdateFrameRepeat();
	void UpdateFrameOnce();

private:
	float GetAngleBetweenVector(const XMVECTOR& vector1, const XMVECTOR& vector2);
	void TurnPlayer(Vec3 vector1, Vec3 vector2);

private:
	void MovePlayerCameraLook(void);
	void ParsePackets();

	void StartRender(network::CPacket& packet);
	void Transform(network::CPacket& packet);

private:
	void SetState(int32_t state);

private:
	server::FBX_TYPE m_playerType;

	PLAYER_STATE m_prevState;
	PLAYER_STATE m_currState;

	float m_radius;
	float m_halfHeight;

	float m_speed = 50.f;

	bool m_aniEnd;
};