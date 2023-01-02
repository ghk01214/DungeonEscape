#include "pch.h"
#include "GameObject.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "MonoBehaviour.h"

CGameObject::CGameObject()
{

}

CGameObject::~CGameObject()
{

}

void CGameObject::Init()
{
	AddComponent(std::make_shared<CTransform>());
}

void CGameObject::Awake()
{
	for (std::shared_ptr<CComponent>& component : m_components)
	{
		if (component)
			component->Awake();
	}

	for (std::shared_ptr<CMonoBehaviour>& script : m_scripts)
	{
		script->Awake();
	}
}

void CGameObject::Start()
{
	for (std::shared_ptr<CComponent>& component : m_components)
	{
		if (component)
			component->Start();
	}

	for (std::shared_ptr<CMonoBehaviour>& script : m_scripts)
	{
		script->Start();
	}
}

void CGameObject::Update()
{
	for (std::shared_ptr<CComponent>& component : m_components)
	{
		if (component)
			component->Update();
	}

	for (std::shared_ptr<CMonoBehaviour>& script : m_scripts)
	{
		script->Update();
	}
}

void CGameObject::LateUpdate()
{
	for (std::shared_ptr<CComponent>& component : m_components)
	{
		if (component)
			component->LateUpdate();
	}

	for (std::shared_ptr<CMonoBehaviour>& script : m_scripts)
	{
		script->LateUpdate();
	}
}

std::shared_ptr<CTransform> CGameObject::GetTransform()
{
	uint8 index = static_cast<uint8>(COMPONENT_TYPE::TRANSFORM);
	return std::static_pointer_cast<CTransform>(m_components[index]);
}

void CGameObject::AddComponent(std::shared_ptr<CComponent> component)
{
	component->SetGameObject(shared_from_this());

	uint8 index = static_cast<uint8>(component->GetType());
	if (index < FIXED_COMPONENT_COUNT)
	{
		m_components[index] = component;
	}
	else
	{
		m_scripts.push_back(std::dynamic_pointer_cast<CMonoBehaviour>(component));
	}
}