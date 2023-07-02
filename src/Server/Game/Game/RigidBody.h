#pragma once
#include "physx_define.h"
#include "Component.h"

class Collider;
class Transform;

class RigidBody : public Component
{
	friend class Collider;

public:
	RigidBody(GameObject* ownerGameObject, Component* ownerComponent);
	~RigidBody();

public:
	void Init() override;
	void Release() override;

public:
	//// 의도한 삭제가 진행되었는지 알기 위해서 타입을 명시하도록 합니다.
	template<typename ColliderType, typename... Args>
	ColliderType* AddCollider(Args&& ...args)
	{
		if (!std::is_base_of<Collider, ColliderType>::value)
		{
			throw std::runtime_error("ColliderType must inherit from Collider\n");
		}

		auto newCollider = new ColliderType(m_ownerGameObject, this, this, std::forward<Args>(args)...);
		newCollider->Init();

		return newCollider;
	}

	template<typename ColliderType>
	ColliderType* GetTypedCollider(int index)
	{
		//error check1
		if (!std::is_base_of<Collider, ColliderType>::value)
		{
			throw std::runtime_error("ColliderType must inherit from Collider\n");
		}

		//error check2
		if (index < 0 || index >= m_colliders.size())
		{
			throw std::runtime_error("Invalid collider index\n");
		}

		Collider* collider = m_colliders[index];

		//error check3
		if (dynamic_cast<ColliderType*>(collider))
		{
			return static_cast<ColliderType*>(collider);
		}
		else
		{
			throw std::runtime_error("GetCollider() at index: " + std::to_string(index) + " does not match ColliderType\n");
		}
	}
	
	template<typename ColliderType>
	void RemoveCollider(ColliderType* colliderToRemove)
	{
		if (!colliderToRemove)
		{
			throw std::runtime_error("Invalid collider pointer\n");
		}

		auto it = std::find_if(m_colliders.begin(), m_colliders.end(), [colliderToRemove](const Collider* collider)
	 {
			 return collider == colliderToRemove;
	 });

		if (it != m_colliders.end())
		{
			ColliderType* collider = dynamic_cast<ColliderType*>(*it);
			if (collider)
			{
				SafeRelease(collider);
				m_colliders.erase(it);
			}
			else
			{
				throw std::runtime_error("ColliderType does not match the type of the collider to remove\n");
			}
		}
		else
		{
			throw std::runtime_error("Collider not found in RigidBody colliders\n");
		}
	}

public:
	void ClearCollidersCollisionInfo();

private:
	//AddComp,RemoveComp에서 작동되는 코드입니다. 외부에서 사용을 금합니다.
	void Attach(Collider* collider);
	void Detach(Collider* collider);
	void ApplyFlags();

public:
	physx::PxRigidDynamic* GetBody();
	Collider* GetCollider(int index);

public:
	physx::PxTransform GetGlobalPose();

	// 프레임워크 구현되는대로 맞게 인자 수정
	physx::PxVec3 GetPosition();
	physx::PxQuat GetRotation();
	void SetPosition(const Vec3 position, bool wake);
	void SetRotation(const Quat rotation);
	void SetRotation(const float degree, PhysicsAxis axis);

	void SetTransform(const Transform* transform);

public:
	physx::PxVec3 GetVelocity() const;
	void SetVelocity(const physx::PxVec3& velocity);
	float GetMass() const;
	void SetMass(float value);
public:
	void AddForce(ForceMode mode, const physx::PxVec3& forceVector);

public:
	void SetAngularDamping(float value);
	void SetLinearDamping(float value);

public:
	void UpdateMassAndInertia();
	bool IsRigidbodySleep() const;
	void SetRigidBodySleep(bool value);
	float GetSleepThresholder() const;
	void SetSleepThresholder(float value);

public:
	void SetRotationLockAxis(PhysicsAxis axes, bool value);
	void SetKinematic(bool value);
	void SetCCDFlag(bool value);
	bool isKinematic() const;

public:
	std::string GetName();
	void SetName(std::string& name);

public:
	void ExcludeFromSimulation(bool value);
	bool IsExcludedFromSimulation();

private:
	physx::PxRigidDynamic* m_body;
	bool m_continuous{ true };

	std::vector<Collider*> m_colliders;
	std::string m_name;
};

