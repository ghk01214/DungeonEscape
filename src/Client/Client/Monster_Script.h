#pragma once

#include "MonoBehaviour.h"

class Monster_Dragon : public MonoBehaviour
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
		WALK_B,
		WALK_L,
		WALK_R,

		DEAD,

		END
	};
public:
	Monster_Dragon();
	~Monster_Dragon();

public:
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

private:
	void CheckState();
	void UpdateFrameRepeat();
	void UpdateFrameOnce();

	void DecideMonsterDeath();

private:
	void ParsePackets();

	void StartRender(network::CPacket& packet);
	void Transform(network::CPacket& packet);
	void ChangeAnimation(network::CPacket& packet);

private:
	DRAGON_STATE m_prevState = IDLE1;
	DRAGON_STATE m_currState = IDLE1;

	int32_t m_hp;

	bool m_recvDead;
};