#include "pch.h"
#include "TriggerPairInfo.h"
#include "Collider.h"

using namespace physx;

TriggerPairInfo::TriggerPairInfo(const PxTriggerPair* pairs, bool inverse)
{
    if (!inverse)
    {
        m_to = static_cast<Collider*>(pairs->triggerShape->userData);
        m_from = static_cast<Collider*>(pairs->otherShape->userData);
    }
    else
    {
        m_to = static_cast<Collider*>(pairs->otherShape->userData);
        m_from = static_cast<Collider*>(pairs->triggerShape->userData);
    }
}

Collider* TriggerPairInfo::GetToCollider() const
{
	return m_to;
}

Collider* TriggerPairInfo::GetFromCollider() const
{
	return m_from;
}