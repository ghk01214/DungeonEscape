#pragma once

#include "MonoBehaviour.h"

class Monster_Weeper : public MonoBehaviour
{
private:
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
	~Monster_Weeper();

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
	WEEPER_STATE m_prevState = IDLE;
	WEEPER_STATE m_currState = IDLE;

	int32_t m_hp;

	bool m_recvDead;
};