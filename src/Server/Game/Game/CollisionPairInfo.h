#pragma once
#include "physx_define.h"

class Collider;
class RigidBody;

struct Contact
{
	physx::PxVec3 point;
	physx::PxVec3 normal;
	physx::PxVec3 impulse;
	float seperation;
};

class CollisionPairInfo
{
public:
	CollisionPairInfo(const physx::PxContactPair* pairs, bool inverse);

public:
	Collider* GetToCollider() const;
	Collider* GetFromCollider() const;
	unsigned int GetContactCount() const;
	Contact GetContact(unsigned int index) const;

private:
	Collider* m_to = nullptr;		//충돌당한 콜라이더
	Collider* m_from = nullptr;		//충돌한 콜라이더

	unsigned int m_numContacts = 0;

	std::vector<physx::PxContactPairPoint> m_contacts;
};

