#include "pch.h"
#include "Component.h"
#include "GameObject.h"

CComponent::CComponent(COMPONENT_TYPE type) : m_type(type)
{

}

CComponent::~CComponent()
{
}

std::shared_ptr<CGameObject> CComponent::GetGameObject()
{
	return m_gameObject.lock();
}

std::shared_ptr<CTransform> CComponent::GetTransform()
{
	return m_gameObject.lock()->GetTransform();
}