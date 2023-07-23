#include "pch.h"
#include "GameInstance.h"
#include "Engine.h"

void CGameInstance::Initialize_Engine(const WindowInfo& info)
{
	m_sceneManager = SceneManager::GetInstance();

	GEngine->Init(info);
}

void CGameInstance::Release_Engine(void)
{
	GEngine->Release();
}

void CGameInstance::LoadScene(std::shared_ptr<CScene> scene)
{
	m_sceneManager->LoadScene(scene);
}

void CGameInstance::SetLayerName(uint8 index, const wstring& name)
{
	m_sceneManager->SetLayerName(index, name);
}

const wstring& CGameInstance::IndexToLayerName(uint8 index)
{
	return m_sceneManager->IndexToLayerName(index);
}

uint8 CGameInstance::LayerNameToIndex(const wstring& name)
{
	return m_sceneManager->LayerNameToIndex(name);
}

shared_ptr<class CGameObject> CGameInstance::Pick(int32 screenX, int32 screenY)
{
	return m_sceneManager->Pick(screenX, screenY);
}

shared_ptr<CScene> CGameInstance::GetActiveScene()
{
	return m_sceneManager->GetActiveScene();
}
