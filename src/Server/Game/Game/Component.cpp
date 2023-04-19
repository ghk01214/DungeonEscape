#include "pch.h"
#include "Component.h"
#include "GameObject.h"

Component::Component(GameObject* ownerGameObject, Component* ownerComponent)
	: m_ownerGameObject(ownerGameObject), m_ownerComponent(ownerComponent)
{
}