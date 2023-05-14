#pragma once

#include "GameObject.h"

class CustomController;

class Player : public GameObject
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
	Player(int32_t playerID = -1, const Vec3& position = Vec3(), const Quat& rotation = Quat(), const Vec3& scale = Vec3(1.f, 1.f, 1.f));
	~Player() override;

public:
	virtual void Init();
	virtual void Update(double timeDelta) override;
	virtual void LateUpdate(double timeDelta) override;
	virtual void Release();

public:
	constexpr int32_t GetPlayerID() const { return m_playerID; }
	constexpr int32_t GetAniIndex() const { return m_aniIndex; }
	constexpr float GetAniFrame() const { return m_aniFrame; }

	void SetAniIndex(int32_t aniIndex);
	void SetAniFrame(float aniFrame);

public:
	void SetControllerMoveSpeed(float value);
	float GetControllerMoveSpeed();
	void SetControllerJumpSpeed(float value);
	float GetControllerJumpSpeed();
public:
	CustomController* m_controller = nullptr;

	int32_t m_playerID = 4294967295U;

	int32_t m_aniIndex;
	float m_aniFrame;
};
