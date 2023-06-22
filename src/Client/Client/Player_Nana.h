#pragma once

#include "MonoBehaviour.h"

class Player_Nana : public MonoBehaviour
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