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
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Models.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Models2.fbx");

	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Stone Bullet1.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Stone Bullet2.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Stone Spike.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Stone.fbx");

	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Pillar Bridge.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Scatter Rock.fbx");

	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Skill\\Ice Ball.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Skill\\Poison Ball.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Skill\\Poison Ball2.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Skill\\Sphere.fbx");

	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character\\Mistic\\Mistic.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character\\Nana\\Nana.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character\\Carmel\\Carmel.fbx");

	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character\\Weeper\\Weeper.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character\\MoltenGolem\\Blue Golem.fbx");
	//GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character\\MoltenGolem\\Green Golem.fbx");
	//GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character\\MoltenGolem\\Red Golem.fbx");
	//GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character\\StylizedScorpion\\Black Scorpion.fbx");
	//GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character\\StylizedScorpion\\Orange Scorpion.fbx");
	//GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character\\StylizedScorpion\\Purple Scorpion.fbx");
	//GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character\\StylizedScorpion\\Red Scorpion.fbx");

	m_isFinished = true;

	return S_OK;
}
