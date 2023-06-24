#pragma once

class Monster;

class Dragon : public Monster
{
public:
	enum DRAGON_STATE
	{
		ATTACK_HAND,
		ATTACK_HORN,
		ATTACK_MOUTH,
		DEFEND,
		DIE,
		DAMAGE,
		IDLE1,
		IDLE2,
		JUMP,
		RUN,
		SCREAM,
		SLEEP,
		WALK,
		WALK_BACK,
		WALK_LEFT,
		WALK_RIGHT,

		DEAD,

		END
	};

public:
	Dragon(int32_t MonsterID = -1, const Vec3& position = Vec3(), const Quat& rotation = Quat(), const Vec3& scale = Vec3(1.f, 1.f, 1.f));
	~Dragon() override;

public:
	void Init() override;
	void Update(double timeDelta) override;
	void LateUpdate(double timeDelta) override;
	void Release() override;

public:
	void CheckState();
	void UpdateFrame();

	void DragonPattern_GroundHit();	//땅을 내리쳐 같은 메쉬에 서있는 오브젝트들을 공중에 띄운다

public:
	DRAGON_STATE GetState() const;

	void SetState(DRAGON_STATE state);

private:
	DRAGON_STATE m_prevState;
	DRAGON_STATE m_currState;
};
