#include "pch.h"
#include "GameInstance.h"
#include "ObjectManager.h"
#include "TimeManager.h"
#include "PhysDevice.h"
#include "TestLevel.h"

ImplementSingletone(GameInstance);

GameInstance::GameInstance()
{
}

GameInstance::~GameInstance()
{
}

void GameInstance::Init()
{
	m_timeManager = TimeManager::GetInstance();
	m_timeManager->Init();
	
	m_objectManager = ObjectManager::GetInstance();
	m_objectManager->Init();

	m_physDevice = PhysDevice::GetInstance();
	m_physDevice->Init();


	//추후 levelManager로 대체
	m_testLevel = new TestLevel;
	m_testLevel->Init();

}

void GameInstance::Update(double timeDelta)
{
	if (nullptr == m_timeManager ||
		nullptr == m_objectManager ||
		nullptr == m_physDevice)
		return;

	//collect pxPairContact
	m_physDevice->PreUpdate();

	m_testLevel->Update(timeDelta);
	m_objectManager->Update(timeDelta);

	//clear Eventcallback > fetch/simulate
	m_physDevice->Update(timeDelta);
}

void GameInstance::LateUpdate(double timeDelta)
{
	if (nullptr == m_objectManager)
		return;

	m_testLevel->LateUpdate(timeDelta);
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

	SafeRelease(m_testLevel);
}
