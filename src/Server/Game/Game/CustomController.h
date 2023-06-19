﻿#pragma once
#include "physx_define.h"
#include "Component.h"

class RigidBody;
class CapsuleCollider;
class Collider;

class CustomController : public Component
{
	enum KEY_ORDER
	{
		W,
		S,
		A,
		D,
		SPACE,
		MAX
	};

	struct KeyInput
	{
		bool down;
		bool press;
		bool up;

		KeyInput() : down{ false }, press{ false }, up{ false } {}

		void None() { down = false; press = false; up = false; }
		void Down() { None(); down = true; }
		void Press() { None(); press = true; }
		void Up() { None(); up = true; }
	};
public:
	CustomController(GameObject* ownerGameObject, Component* ownerComponent, bool player);
	~CustomController();
public:
	void Init() override;
	void Release() override;
public:
	//CustomController 컴포넌트를 사용하는 게임오브젝트는 Move()를 GameObject::Update()에서 반드시 호출
	void Move(int32_t objID);
	bool CheckOnGround(CollisionInfoType type, physx::PxVec3& surfaceNormal);
	void GetSlidingVector(CollisionInfoType type);
	bool CheckOnGround_Raycast();
	Collider* GetColliderBelow();
	void DirectionInput_Player();
	void DirectionInput_Monster();
	void Movement_Player(int32_t objID);
	void Movement_Monster();

public:
	void ClearControllerCollisionInfo();

public:
	void CameraLookReceive(Vec3& CameraLook);
	void KeyboardReceive(ulong32_t key);

public:
	void GetKeyboardStatus();	//Receive와 다른, player코드에서 키보드 정보를 얻기 위한 함수

public:
	void SetMoveSpeed(float value);
	float GetMoveSpeed();
	void SetJumpSpeed(float value);
	float GetJumpSpeed();
	float GetDistanceFromGround();

public:
	void BounceFromAttack();	//호출시키지 않으면 공격넉백 적용이 불가능하다

public:
	RigidBody* GetBody();
	CapsuleCollider* GetCollider();
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
	KeyInput		m_keyboardLeft;
	KeyInput		m_keyboardRight;
	KeyInput		m_keyboardUp;
	KeyInput		m_keyboardDown;
	KeyInput		m_keyboardSpace;
#pragma endregion
	bool m_isPlayer = false;
	physx::PxVec3	m_cameraLook = physx::PxVec3(1, 0, 0);
};


