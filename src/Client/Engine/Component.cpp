#include "pch.h"
#include "Component.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Animator.h"
#include "Network.h"

Component::Component(COMPONENT_TYPE type) : Object(OBJECT_TYPE::COMPONENT), m_type(type)
{

}

Component::~Component()
{
}

shared_ptr<CGameObject> Component::GetGameObject()
{
	return m_gameObject.lock();
}

shared_ptr<Transform> Component::GetTransform()
{
	return m_gameObject.lock()->GetTransform();
}

shared_ptr<MeshRenderer> Component::GetMeshRenderer()
{
	return m_gameObject.lock()->GetMeshRenderer();
}

shared_ptr<Animator> Component::GetAnimator()
{
	return m_gameObject.lock()->GetAnimator();
}

shared_ptr<CNetwork> Component::GetNetwork()
{
	return m_gameObject.lock()->GetNetwork();
}