#pragma once

#include "GameObject.h"

class RigidBody;
class Event;

class SkillObject : public GameObject
{
	friend class Player;
	friend class Monster;
	friend class Weeper;
	friend class Event;

public:
	enum class SKILLOBJECTTYPE
	{
		PLAYER_FIREBALL,
		PLAYER_ICEBALL,
		PLAYER_THUNDERBALL,
		PLAYER_POISONBALL,
		PLAYER_METEOR,

		BOUNDARY,

		WEEPER_CAST1_BALL,
		WEEPER_CAST2_BALL,
		WEEPER_CAST2_BALL_SCATTER,
		WEEPER_CAST2_BALL_NUCLEAR,
		WEEPER_CAST3_BALL,
		WEEPER_CAST4_BALL,

		MONSTER_ICEBALL,
		MONSTER_THUNDERBALL,
		MONSTER_POISONBALL,
		MONSTER_DARKBALL
	};

	enum SKILLATTRIBUTE
	{
		NONE			= 0,
		LEVITATE		= 1 << 0,
		GUIDED			= 1 << 1,
		ASCENDING		= 1 << 2,
		DESCENDING		= 1 << 3,
		NUCLEAR			= 1 << 4,
		GUIDED_METEOR   = 1 << 5,
		NEW_LEVITATE    = 1 << 6
	};

public:
	SkillObject(const Vec3& position, const Quat& rotation, const Vec3& scale, SKILLOBJECTTYPE skilltype, GameObject* target, GameObject* owner);
	~SkillObject() override;

public:
	virtual void Init();
	virtual void Update(double timeDelta) override;
	virtual void LateUpdate(double timeDelta) override;
	virtual void Release();

public:
	void ServerMessage_Init();
	void ServerMessage_Add();
	void ServerMessage_Transform();
	void ServerMessage_Hit();
	void ServerMessage_Remove();
	void ServerMessage_Effect(Vec3 pos);

public:
	void HandlePlayerSkillCollision();							//플레이어 스킬의 충돌 판단	(피격 : 몬스터)
	void HandleMonsterSkillCollision();							//몬스터 스킬의 충돌 판단		(피격 : 플레이어)

public:
	void PlayerSkillFire(physx::PxVec3 dir);					//Player::PlayerPattern_ShootBall()에서 스킬 오브젝트를 발사하기 위한 함수

	void MonsterSkillFire(physx::PxVec3 dir);					//Weeper::Pattern_Cast1()에서 스킬 오브젝트를 발사하기 위한 함수
	void WeeperNuclearFire();
public:
	void Handle_Attribute();		//attribute에 따라 지속적으로 실행
	void Attirbute_Levitate();		//공중 지속
	void Attirbute_New_Levitate();		//공중 지속
	void Attribute_Guide();			//유도
	void Attribute_Guide_MeteorOnly();
	void Attribute_Ascending();		//공중 발사
	void Attribute_Descending();

	void SetAttribute(SKILLATTRIBUTE attrib, bool set);
	void SetSkillObjectType(SKILLOBJECTTYPE type);
	SKILLATTRIBUTE GetAttribute();

public:
	void Nuclear_Attribute_Explosion();

public:
	bool IsPlayerSkill();

private:
	RigidBody* m_body = nullptr;
	SKILLOBJECTTYPE m_skillType;
	SKILLATTRIBUTE m_skillAttrib = SKILLATTRIBUTE::NONE;
	float m_firePower;
	bool m_mapEncountered = false;		//맵 1회 접촉 이후 삭제

	GameObject* m_target = nullptr;
	GameObject* m_owner = nullptr;

	bool m_flagPlayer = false;
};



