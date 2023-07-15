#include "pch.h"
#include "Loader.h"
#include "Resources.h"
#include "MeshData.h"

shared_ptr<CLoader> CLoader::Create(SCENE eScene)
{
	shared_ptr<CLoader> pInstance = make_shared<CLoader>();

	if (FAILED(pInstance->Loading(eScene)))
	{
		MSG_BOX(TEXT("Failed to Created CLoader"));
	}

	return pInstance;
}

unsigned int APIENTRY ThreadEntryFunc(CLoader* pArg)
{
	//shared_ptr<CLoader> pLoader = static_cast<shared_ptr<CLoader>>(pArg);

	//shared_ptr<CLoader> pLoader = pArg;

	CLoader* pLoader = (CLoader*)pArg;

	EnterCriticalSection(pLoader->Get_CriticalSection());

	switch (pLoader->Get_SceneIndex())
	{
		case SCENE_GAMEPLAY:
		pLoader->Loading_ForGamePlayLevel();
		break;
	}

	LeaveCriticalSection(pLoader->Get_CriticalSection());

	return 0;
}

CLoader::CLoader(void)
{
}

HRESULT CLoader::Loading(SCENE eScene)
{
	m_eScene = eScene;

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	InitializeCriticalSection(&m_CriticalSection);

	thread loaderThread = std::thread{ ThreadEntryFunc, this };

	loaderThread.join();

	//m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadEntryFunc, this, 0, nullptr);
	//if (0 == m_hThread)
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlayLevel()
{
	// 여기서 리소스 추가

	m_isFinished = false;

	// 폰트

	// 텍스쳐
	GET_SINGLE(Resources)->LoadTextures(L"Effect_Fire", L"..\\Resources\\Texture\\Effect\\Fire\\Fire.png", 64);

	// fbx 모델
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character\\Mistic\\Mistic.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Environments\\Bones.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Environments\\Floors.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Environments\\Misc.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Environments\\Optimized.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Environments\\Pillars.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Environments\\Rocks.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Environments\\Walls.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Environments\\Wood.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Environments\\Items.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Environments\\Props.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Environments\\Weapons.fbx");

	m_isFinished = true;

	return S_OK;
}
