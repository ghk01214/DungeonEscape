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

#include "SoundManager.h"

void CGame::Init(const WindowInfo& Info)
{
	GGameInstance->Initialize_Engine(Info);

	//shared_ptr<CScene> pScene = Scene_Test::Create(magic_enum::enum_value<server::FBX_TYPE>(classNum));

	Open_Scene(SCENE_CHARACTER_SELECT);
	//Open_Scene(SCENE_START);

	//GET_SINGLE(SceneManager)->LoadScene(pScene);

	//GET_NETWORK->SendLoginPacket(ID, PWD);
}

void CGame::Start()
{
	//GET_NETWORK->SendIDIssueRequest();
	//GET_SINGLE(SceneManager)->Start();

	//GET_SINGLE(CSoundMgr)->PlayBGM(L"Blue Archive Main Title Constant Moderato Piano Vocal.mp3");
}

void CGame::Update()
{
	GEngine->Update();
}

void CGame::Release(void)
{
	GEngine->Release();
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
