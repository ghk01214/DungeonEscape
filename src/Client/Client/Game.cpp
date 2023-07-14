#include "pch.h"
#include "Game.h"
#include "Engine.h"
#include "SceneManager.h"
#include <NetworkManager.h>

#include "GameInstance.h"
#include "Scene_Test.h"
#include "Player_Script.h"

#include "Scene_Logo.h"
#include "Scene_Loading.h"

void CGame::Init(const WindowInfo& Info, const std::wstring& ID, const std::wstring& PWD)
{
	GGameInstance->Initialize_Engine(Info);

	int32_t classNum{ 2 };
	//std::wcout << std::format(L"Warrior = 1\n");
	//std::wcout << std::format(L"Magician = 2\n");
	//std::wcout << std::format(L"Priest = 3\n");
	//std::wcout << std::format(L"Choose player class : \n");
	//std::wcin >> classNum;

	//shared_ptr<CScene> pScene = Scene_Test::Create(magic_enum::enum_value<server::FBX_TYPE>(classNum));

	Open_Scene(SCENE_LOGO);

	//GET_SINGLE(SceneManager)->LoadScene(pScene);

	GET_NETWORK->SendLoginPacket(ID, PWD);
}

void CGame::Start()
{
	//GET_NETWORK->SendIDIssueRequest();
	//GET_SINGLE(SceneManager)->Start();
}

void CGame::Update()
{
	GEngine->Update();
}

void CGame::Open_Scene(SCENE eSceneIndex)
{
	shared_ptr<CScene> pScene = nullptr;

	switch (eSceneIndex)
	{
		case SCENE_LOGO:
		pScene = Scene_Logo::Create();
		break;

		default:
		pScene = Scene_Loading::Create(eSceneIndex);
		break;
	}

	GET_SINGLE(SceneManager)->LoadScene(pScene);
}
