#include "pch.h"
#include "GameInstance.h"
#include "ObjectManager.h"
#include "TimeManager.h"
#include "PhysDevice.h"

ImplementSingletone(GameInstance);

void GameInstance::Init()
{
	m_timeManager = TimeManager::GetInstance();
	m_timeManager->Init();
	
	m_objectManager = ObjectManager::GetInstance();
	m_objectManager->Init();

	m_physDevice = PhysDevice::GetInstance();
	m_physDevice->Init();
}

void GameInstance::Update(double timeDelta)
{
	if (nullptr == m_timeManager ||
		nullptr == m_objectManager ||
		nullptr == m_physDevice)
		return;

	m_physDevice->PreUpdate();
	m_objectManager->Update(timeDelta);
	m_physDevice->Update(timeDelta);
}

void GameInstance::LateUpdate(double timeDelta)
{
	if (nullptr == m_objectManager)
		return;

	m_objectManager->LateUpdate(timeDelta);
	m_physDevice->LateUpdate(timeDelta);
}

void GameInstance::Release()
{
	ObjectManager::GetInstance()->DestroyInstance();
	m_objectManager = nullptr;
	
	TimeManager::GetInstance()->DestroyInstance();
	m_objectManager = nullptr;

	PhysDevice::GetInstance()->DestroyInstance();
	m_physDevice = nullptr;
}
