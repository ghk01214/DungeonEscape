#include "pch.h"
#include "GameInstance.h"
#include "ObjectManager.h"
#include "TimeManager.h"
#include "PhysDevice.h"
#include "EventHandler.h"
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

	m_eventHandler = EventHandler::GetInstance();
	m_eventHandler->Init();


	//추후 levelManager로 대체
	m_testLevel = new TestLevel;
	m_testLevel->Init();

}

void GameInstance::Update(double timeDelta, int interExtrapolation)
{
	if (nullptr == m_timeManager ||
		nullptr == m_objectManager ||
		nullptr == m_physDevice)
		return;

	if (interExtrapolation == 0)
		return;							//interpolation

	//contactPair Notify
	m_physDevice->PreUpdate();			//fetch/sim n회만큼의 결과값이 담겨있다. 그 값을 timescale 주기마다 받아들인다.

	m_testLevel->Update(timeDelta);		//
	m_objectManager->Update(timeDelta);	//
	//m_eventHandler->Update(timeDelta);	//timescale 주기마다 1회 업데이트

	//clear Eventcallback > fetch/simulate
	m_physDevice->Update(timeDelta, interExtrapolation);	//fetch > sim은 초과한 주기만큼 수행한다.
}

void GameInstance::LateUpdate(double timeDelta, int interExtrapolation)
{
	if (nullptr == m_objectManager)
		return;

	m_testLevel->LateUpdate(timeDelta);
	m_objectManager->LateUpdate(timeDelta);			//timescale 주기마다 notify > clearInfo 루프를 돈다.
	//m_eventHandler->LateUpdate(timeDelta);

	m_physDevice->LateUpdate(timeDelta);	
}

void GameInstance::Release()
{
	SafeRelease(m_testLevel);

	ObjectManager::GetInstance()->DestroyInstance();
	m_objectManager = nullptr;

	TimeManager::GetInstance()->DestroyInstance();
	m_timeManager = nullptr;

	PhysDevice::GetInstance()->DestroyInstance();
	m_physDevice = nullptr;

	EventHandler::GetInstance()->DestroyInstance();
	m_eventHandler = nullptr;
}
