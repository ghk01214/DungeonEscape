#pragma once
#include "physx_define.h"
#include "Component.h"

enum KEY_ORDER
{
	W,
	S,
	A,
	D,
	R,
	SPACE,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,

	MAX
};

struct KeyInput
{
	KeyInput() : down{ false }, press{ false }, up{ false } {}

	void None() { down = false; press = false; up = false; }
	void Down() { None(); down = true; }
	void Press() { None(); press = true; }
	void Up() { None(); up = true; }

	bool down;
	bool press;
	bool up;
};

class RigidBody;
class CapsuleCollider;
class Collider;

class CustomController : public Component
{
public:
	CustomController(GameObject* ownerGameObject, Component* ownerComponent, bool player);
	~CustomController();
public:
	void Init() override;
	void Release() override;
public:
	//CustomController 컴포넌트를 사용하는 게임오브젝트는 Move()를 GameObject::Update()에서 반드시 호출
	void PlayerMove();
	void MonsterMove(physx::PxVec3 targetDir);
	bool CheckOnGround(CollisionInfoType type, physx::PxVec3& surfaceNormal);
	void GetSlidingVector(CollisionInfoType type);
	bool CheckOnGround_Raycast();
	Collider* GetColliderBelow();
	void DirectionInput_Player();
	void DirectionInput_Monster(physx::PxVec3 targetDir);
	void Movement_Player();
	void Movement_Monster();

public:
	void ClearControllerCollisionInfo();

public:
	void CameraLookReceive(Vec3& CameraLook);
	void KeyboardReceive(ulong32_t key);
	KeyInput& GetKeyStatus(KEY_ORDER key);
	KeyInput& GetKeyStatus(int32_t key);
	std::vector<KeyInput>& GetKeyInput();
	void Keyboard_Direction_Clear();		//FSM에서 directionInput()에 제한을 걸기 위한 함수
	void Keyboard_SpaceBar_Clear();			//FSM에서 move()에 제한을 걸기 위한 함수
	void Keyboard_ATK_Clear();				//FSM에서 공격 키보드를 비우기 위한 함수


public:
	void SetMoveSpeed(float value);
	float GetMoveSpeed();
	void SetJumpSpeed(float value);
	float GetJumpSpeed();
	float GetDistanceFromGround();
	bool Falling();


public:
	void BounceFromAttack();	//호출시키지 않으면 공격넉백 적용이 불가능하다

public:
	RigidBody* GetBody();
	CapsuleCollider* GetCollider();
	bool IsOnGroundByDistance();
	bool IsOnGround();
	physx::PxVec3 GetCameraLook();

private:
	physx::PxVec3	m_moveDirection{ 0.f };
	physx::PxVec3	m_slidingVector{ 0.f };

	bool	m_onGround = false;
	float	m_distanceFromGround = -1.f;		// -1.f값이 디폴트


	float	m_degreeThreshold = 49.9f;
	float	m_moveSpeed = 30.f;
	float	m_jumpSpeed = 70.f;

	bool	m_BounceFromAttack = false;
private:
	RigidBody* m_body = nullptr;
	CapsuleCollider* m_collider = nullptr;

#pragma region keyboardVariable
	std::vector<server::KEY_TYPE> m_useKeyType;
	std::vector<KeyInput> m_keyboardInput;
#pragma endregion
	bool m_isPlayer = false;
	physx::PxVec3	m_cameraLook = physx::PxVec3(1, 0, 0);
};


