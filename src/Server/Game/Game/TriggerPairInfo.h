#pragma once
#include "physx_define.h"

class Collider;
class RigidBody;

class TriggerPairInfo
{
public:
	TriggerPairInfo(const physx::PxTriggerPair* pairs, bool inverse);

public:
	Collider* GetToCollider() const;
	Collider* GetFromCollider() const;

private:
	Collider* m_to = nullptr;		//충돌당한 콜라이더
	Collider* m_from = nullptr;		//충돌한 콜라이더
};

