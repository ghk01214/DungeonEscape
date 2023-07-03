#pragma once

#include "GameObject.h"

class RigidBody;

class SkillObject : public GameObject
{
	friend class Player;
	friend class Monster;
	friend class Weeper;

public:
	enum class SKILLOBJECTTYPE
	{
		PLAYER_FIREBALL,
		PLAYER_ICEBALL,
		PLAYER_THUNDERBALL,
		PLAYER_POISONBALL,

		BOUNDARY,

		MONSTER_FIREBALL,
		MONSTER_ICEBALL,
		MONSTER_THUNDERBALL,
		MONSTER_POISONBALL,
		MONSTER_DARKBALL
	};

public:
	SkillObject(const Vec3& position, const Quat& rotation, const Vec3& scale, SKILLOBJECTTYPE skilltype);
	~SkillObject() override;

public:
	virtual void Init();
	virtual void Update(double timeDelta) override;
	virtual void LateUpdate(double timeDelta) override;
	virtual void Release();

public:
	void ServerMessage_SkillInit();
	void ServerMessage_SkillHit();

public:
	bool IsPlayerAttribute();

public:
	void PlayerSkillFire(physx::PxVec3 dir);					//Player::PlayerPattern_ShootBall()에서 스킬 오브젝트를 발사하기 위한 함수

public:
	void HandlePlayerSkillCollision();		//플레이어 스킬의 충돌 판단	(피격 : 몬스터)
	void HandleMonsterSkillCollision();		//몬스터 스킬의 충돌 판단		(피격 : 플레이어)
private:
	RigidBody* m_body = nullptr;
	SKILLOBJECTTYPE m_skillType;
	float m_firePower;
};



