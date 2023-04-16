#include "pch.h"
#include "Component.h"
#include "GameObject.h"

Component::Component(GameObject* ownerGameObject, Component* ownerComponent)
	: m_ownerGameObject(ownerGameObject), m_ownerComponent(ownerComponent)
{
}



template<typename T, typename... Args>
T* Component::AddComponent(Args&&... args)
{
	T* component = new T(m_ownerGameObject, this, std::forward<Args>(args)...);
	component->init();
	return component;
}
	