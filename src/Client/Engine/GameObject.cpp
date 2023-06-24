#include "pch.h"
#include "GameObject.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "Light.h"
#include "MonoBehaviour.h"
#include "ParticleSystem.h"
#include "Terrain.h"
#include "BaseCollider.h"
#include "Animator.h"
#include "Network.h"

CGameObject::CGameObject() : Object(OBJECT_TYPE::GAMEOBJECT)
{
	m_objectType = server::OBJECT_TYPE::NONE;
}

CGameObject::~CGameObject()
{

}

void CGameObject::Awake()
{
	for (shared_ptr<Component>& component : m_components)
	{
		if (component)
			component->Awake();
	}

	for (shared_ptr<MonoBehaviour>& script : m_scripts)
	{
		script->Awake();
	}
}

void CGameObject::Start()
{
	for (shared_ptr<Component>& component : m_components)
	{
		if (component)
			component->Start();
	}

	for (shared_ptr<MonoBehaviour>& script : m_scripts)
	{
		script->Start();
	}
}

void CGameObject::Update()
{
	for (shared_ptr<Component>& component : m_components)
	{
		if (component)
			component->Update();
	}

	for (shared_ptr<MonoBehaviour>& script : m_scripts)
	{
		script->Update();
	}
}

void CGameObject::LateUpdate()
{
	for (shared_ptr<Component>& component : m_components)
	{
		if (component)
			component->LateUpdate();
	}

	for (shared_ptr<MonoBehaviour>& script : m_scripts)
	{
		script->LateUpdate();
	}
}

void CGameObject::FinalUpdate()
{
	for (shared_ptr<Component>& component : m_components)
	{
		if (component)
			component->FinalUpdate();
	}
}

shared_ptr<Component> CGameObject::GetFixedComponent(COMPONENT_TYPE type)
{
	uint8 index = static_cast<uint8>(type);
	assert(index < FIXED_COMPONENT_COUNT);
	return m_components[index];
}

shared_ptr<Transform> CGameObject::GetTransform()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::TRANSFORM);
	return static_pointer_cast<Transform>(component);
}

shared_ptr<MeshRenderer> CGameObject::GetMeshRenderer()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::MESH_RENDERER);
	return static_pointer_cast<MeshRenderer>(component);
}

shared_ptr<Camera> CGameObject::GetCamera()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::CAMERA);
	return static_pointer_cast<Camera>(component);
}

shared_ptr<Light> CGameObject::GetLight()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::LIGHT);
	return static_pointer_cast<Light>(component);
}

shared_ptr<ParticleSystem> CGameObject::GetParticleSystem()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::PARTICLE_SYSTEM);
	return static_pointer_cast<ParticleSystem>(component);
}

shared_ptr<Terrain> CGameObject::GetTerrain()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::TERRAIN);
	return static_pointer_cast<Terrain>(component);
}

shared_ptr<BaseCollider> CGameObject::GetCollider()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::COLLIDER);
	return static_pointer_cast<BaseCollider>(component);
}

shared_ptr<Animator> CGameObject::GetAnimator()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::ANIMATOR);
	return static_pointer_cast<Animator>(component);
}

shared_ptr<network::CNetwork> CGameObject::GetNetwork()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::NETWORK);
	return static_pointer_cast<network::CNetwork>(component);
}

void CGameObject::AddComponent(shared_ptr<Component> component)
{
	if (nullptr == component)
		return;

	component->SetGameObject(shared_from_this());

	uint8 index = static_cast<uint8>(component->GetType());
	if (index < FIXED_COMPONENT_COUNT)
	{
		m_components[index] = component;
	}
	else
	{
		m_scripts.push_back(dynamic_pointer_cast<MonoBehaviour>(component));
	}
}