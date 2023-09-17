#pragma once

#include "GameObject.h"

class CustomController;
class TriggerObject;
class OverlapObject2;
class MonsterAI;
class Monster;

class Player : public GameObject
{
	friend class SkillObject;
public:
	enum PLAYER_STATE
	{
		AERT,			// 0.96
		ATK0,			// 0.8(공격 0.24)
		ATK1,			// 0.8(공격 0.24)
		ATK2,			// 0.8(공격 0.12)
		ATK3,			// 0.8(공격 0.64)
		ATK4,			// 0.8(공격 0.12)

		BLOCK,			// 0.8
		DAMAGE,			// 0.8

		DASH,			// 1.0
		DIE0,			// 2.0
		DIE1,			// 2.0
		DIE2,			// 1.2
		IDLE1,			// 1.24
		IDLE2,			// 2.8
		IDLE3,			// 4.4
		JUMP_START,		// 1.2
		MOVE,			// 0.8
		MOVE_LEFT,		// 0.8
		MOVE_RIGHT,		// 0.8
		REST,			// 3.04
		RUN,			// 0.52
		RUN_LEFT,		// 0.52
		RUN_RIGHT,		// 0.52
		SHOOT,			// 0.64 (공격 0.28)
		SLEEP,			// 2.0
		SWOON,			// 기절 1.2
		TIRED,			// 1.0
		VICTORY1,		// 0.8
		VICTORY2,		// 1.04
		WALK,			// 1.0
		WALK_LEFT,		// 1.0
		WALK_RIGHT,		// 1.0

		JUMPING,
		JUMP_END,
		DEAD,

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
	constexpr int32_t GetHP() const { return m_hp; }
	constexpr int32_t GetMP() const { return m_mp; }

	void IsOnGround();
	bool IsDead();

	void TriggerZoneStatusChange(server::TRIGGER_TYPE triggerType, bool status);
	void TriggerZoneStatusUpdate();
	void Trigger_Magma();
	void Trigger_Wind();
	void Trigger_SingleStrike();

	PLAYER_STATE GetState() const;
	float GetAniPlayTime() const;

	void SetState(PLAYER_STATE state);
	void SetState(int32_t index);
	void SetAniPlayTime(float time);
	void SetAniEndFlag(bool flag);
	void SetPlayerQuat(Quat& quat);

	void GetDamaged(int32_t damage);
	void CalculateCameraDistance();

public:
	void KeyboardLimit();
	void ChangeStateByKeyInput();
	void State_Check_Enter();
	void Update_Frame_Continuous();
	void Update_Frame_Once();
	void PlayerMove();				//controller를 사용해 이동한다. state가 move/jump가 아닐 시 키보드 정보 삭제
	void SkillAttempt();
	void DeathCheck();				//hp를 따져 DIE0으로 진입

	void SetStun(bool value);
	void StunCheck();

	Vec3 LocationForBilboard_VictimPlayer(MonsterAI* overlapOwner);
	Vec3 LocationForBilboard_VictimMonster(Monster* monster);

public:
	CustomController* GetController();
	void SetControllerPosition(Vec3 pos);
	Vec3 GetControllerPosition();
	Quat GetControllerRotation();
	float GetControllerMoveSpeed();
	float GetControllerJumpSpeed();
	float GetCameraDistance();
	void SetControllerMoveSpeed(float value);
	void SetControllerJumpSpeed(float value);
	void SetControllerCameraLook(Vec3& value);
	void SetMeteorAttackAvailable(bool value);
	void SetCameraDistance(float value);

public:
	void Set_OverlapObject(bool activate);
	physx::PxQuat GetRotation_For_Overlap(physx::PxVec3 xzDir);

private:
	void PlayerPattern_ShootBall();
	void PlayerPattern_ShootMeteor();
	void PlayerPattern_SingleStrike(PLAYER_STATE state);
	void PlayerPattern_ATTACK_ForDebug();
	int32_t IsAttackKeyDown();

	void ServerMessage_SendTransform();
	physx::PxVec3 GetForwardVec();


private:
	CustomController* m_controller = nullptr;
	OverlapObject2* m_overlapObj = nullptr;

	PLAYER_STATE m_prevState;
	PLAYER_STATE m_currState;

	float m_aniPlayTime;
	bool m_aniEnd;

	std::unordered_map<server::TRIGGER_TYPE, bool> m_triggerDictionary;

	int32_t m_hp;
	int32_t m_mp;

	bool m_firstSingleStrike;
	bool m_banTriggerApproach = false;		//trigger삭제 후 접근방지
	bool m_prevOnGround;

	bool m_stun = false;

	bool m_meteorAvailable = true;			//추후 아이템 획득 등으로 조건 변경

	float m_cameraDistance;					// 이 변수 값을 바꿔주면 자동으로 카메라 거리를 클라이언트로 전달

private:
	Quat m_qlook;
};
