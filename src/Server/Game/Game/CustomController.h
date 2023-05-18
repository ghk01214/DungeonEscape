#pragma once
#include "physx_define.h"
#include "Component.h"

class RigidBody;
class CapsuleCollider;

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
public:
	CustomController(GameObject* ownerGameObject, Component* ownerComponent);
	~CustomController();
public:
	void Init() override;
	void Release() override;
public:
	//CustomController 컴포넌트를 사용하는 게임오브젝트는 Move()를 GameObject::Update()에서 반드시 호출
	void Move();
	bool CheckOnGround(CollisionInfoType type, physx::PxVec3& surfaceNormal);
	void GetSlidingVector(CollisionInfoType type);
	bool CheckOnGround_Raycast();
	void DirectionInput();
	void Movement();

public:
	void ClearControllerCollisionInfo();

public:
	void CameraLookReceive(Vec3& CameraLook);
	void KeyboardReceive(ulong32_t key);
	void KeyboardClear();

public:
	void SetMoveSpeed(float value);
	float GetMoveSpeed();
	void SetJumpSpeed(float value);
	float GetJumpSpeed();

public:
	RigidBody* GetBody();
	CapsuleCollider* GetCollider();
	bool IsOnGround();
	bool IsStartJump();

private:
	physx::PxVec3	m_moveDirection{ 0.f };
	physx::PxVec3	m_slidingVector{ 0.f };

	bool	m_onGround = false;
	float	m_degreeThreshold = 49.9f;
	float	m_moveSpeed = 300.f;
	float	m_jumpSpeed = 70.f;
private:
	RigidBody*			m_body = nullptr;
	CapsuleCollider* m_collider = nullptr;

#pragma region keyboardVariable
	std::vector<server::KEY_TYPE> m_useKeyType;
	bool			m_keyboardLeft = false;
	bool			m_keyboardRight = false;
	bool			m_keyboardUp = false;
	bool			m_keyboardDown = false;
	bool			m_keyboardSpace = false;
#pragma endregion

	physx::PxVec3	m_cameraLook;

	bool m_startJump;
};


