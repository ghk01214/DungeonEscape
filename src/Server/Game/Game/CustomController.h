#pragma once
#include "physx_define.h"
#include "Component.h"

class RigidBody;
class CapsuleCollider;

class CustomController : public Component
{
public:
	CustomController(GameObject* ownerGameObject, Component* ownerComponent);
	~CustomController();
public:
	void Init() override;
	void Release() override;
public:
	//CustomController 컴포넌트를 사용하는 게임오브젝트는 Move()를 GameObject::Update()에서 반드시 호출
	void Move(uint8_t keyType, server::KEY_STATE keyState);
	bool CheckOnGround(CollisionInfoType type, physx::PxVec3& surfaceNormal);
	void GetSlidingVector(CollisionInfoType type);
	bool CheckOnGround_Raycast();
	void DirectionInput(uint8_t keyType);
	void Movement(uint8_t keyType, server::KEY_STATE keyState);

public:
	void ClearControllerCollisionInfo();
private:
	physx::PxVec3	m_moveDirection{ 0.f };
	physx::PxVec3	m_slidingVector{ 0.f };

	bool	m_onGround = false;
	float	m_degreeThreshold = 49.9f;
private:
	RigidBody*			m_body = nullptr;
	CapsuleCollider* m_collider = nullptr;
};


