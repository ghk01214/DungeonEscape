#include "pch.h"
#include "BaseCollider.h"

BaseCollider::BaseCollider(ColliderType colliderType)
	: Component(COMPONENT_TYPE::COLLIDER), m_colliderType(colliderType)
{

}

BaseCollider::~BaseCollider()
{

}