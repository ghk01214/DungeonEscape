﻿#pragma once

class Monster;

class Weeper : public Monster
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
	Weeper(int32_t MonsterID = -1, const Vec3& position = Vec3(), const Quat& rotation = Quat(), const Vec3& scale = Vec3(1.f, 1.f, 1.f));
	~Weeper() override;

public:
	void Init() override;
	void Update(double timeDelta) override;
	void LateUpdate(double timeDelta) override;
	void Release() override;

public:
	void CheckState();
	void UpdateFrame();

public:
	WEEPER_STATE GetState() const;

	void SetState(WEEPER_STATE state);

private:
	WEEPER_STATE m_prevState;
	WEEPER_STATE m_currState;
};