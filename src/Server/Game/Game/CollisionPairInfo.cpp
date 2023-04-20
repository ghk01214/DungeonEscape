#include "pch.h"
#include "CollisionPairInfo.h"

CollisionPairInfo::CollisionPairInfo(const physx::PxContactPair* pairs, bool inverse)
{
	// =====================================================================================================
	// pairs.normal 방향은 shape1-> shape0의 방향
	// 
	//CapsuleCollider* capsule = nullptr;
	//Collider* collider = nullptr;
	//dynamic_cast<CapsuleCollider*>(collider);
	// =====================================================================================================
	m_numContacts = pairs->contactCount;
	m_contacts.resize(m_numContacts);
	pairs->extractContacts(m_contacts.data(), m_numContacts);

	if (!inverse)
	{
		void* ptr = pairs->shapes[0]->userData;
		static_cast<Collider*>(ptr);
		m_to = (Collider*)pairs->shapes[0]->userData;
		m_from = (Collider*)pairs->shapes[1]->userData;
	}
	else
	{
		m_to = (Collider*)pairs->shapes[1]->userData;
		m_from = (Collider*)pairs->shapes[0]->userData;
		for (unsigned int i = 0; i < m_numContacts; ++i)
		{
			m_contacts[i].normal *= -1;
		}
	}
}

Collider* CollisionPairInfo::GetToCollider() const
{
	return m_to;
}

Collider* CollisionPairInfo::GetFromCollider() const
{
	return m_from;
}

unsigned int CollisionPairInfo::GetContactCount() const
{
	return m_numContacts;
}

Contact CollisionPairInfo::GetContact(unsigned int index) const
{
	Contact contact;
	contact.point = m_contacts[index].position;
	contact.normal = m_contacts[index].normal;
	contact.impulse = m_contacts[index].impulse;
	return contact;
}