#include "pch.h"
#include "GameObject.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Camera.h"

#include "MonoBehaviour.h"

CGameObject::CGameObject()
{

}

CGameObject::~CGameObject()
{

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

void CGameObject::FinalUpdate()
{
	for (std::shared_ptr<CComponent>& component : m_components)
	{
		if (component)
			component->FinalUpdate();
	}
}

std::shared_ptr<CComponent> CGameObject::GetFixedComponent(COMPONENT_TYPE type)
{
	uint8 index = static_cast<uint8>(type);
	assert(index < FIXED_COMPONENT_COUNT);
	return m_components[index];
}

std::shared_ptr<CTransform> CGameObject::GetTransform()
{
	std::shared_ptr<CComponent> component = GetFixedComponent(COMPONENT_TYPE::TRANSFORM);
	return std::static_pointer_cast<CTransform>(component);
}

std::shared_ptr<CMeshRenderer> CGameObject::GetMeshRenderer()
{
	std::shared_ptr<CComponent> component = GetFixedComponent(COMPONENT_TYPE::MESH_RENDERER);
	return std::static_pointer_cast<CMeshRenderer>(component);
}

std::shared_ptr<CCamera> CGameObject::GetCamera()
{
	std::shared_ptr<CComponent> component = GetFixedComponent(COMPONENT_TYPE::CAMERA);
	return std::static_pointer_cast<CCamera>(component);
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